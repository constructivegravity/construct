#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <common/task_pool.hpp>
#include <tensor/permutation.hpp>
#include <tensor/fraction.hpp>
#include <tensor/symmetry.hpp>

namespace Construction {
	namespace Tensor {

		// Forward declarations
		class ScaledTensor;
		class AddedTensor;
		class MultipliedTensor;

		/**
			\class CannotAddTensorsException
		 */
		class CannotAddTensorsException : public Exception {
		public:
			CannotAddTensorsException() : Exception("Cannot add tensors due to incompatible indices") { }
		};

		/**
			\class CannotMultiplyTensorsException
		 */
		class CannotMultiplyTensorsException : public Exception {
		public:
			CannotMultiplyTensorsException() : Exception("Cannot multiply tensors due to incompatible indices") { }
		};

		// Evaluation function
		typedef std::function<double(const std::vector<unsigned>&)>	EvaluationFunction;

		/**
			\class Tensor

		 	\brief Class for general tensors

		 	The general class for a tensor. A tensor consists of a name, a printable LaTeX
		 	text and especially a set of indices. In general the index assignment,
		 	accessed by the functor operator, first checks if the index
		 	assignment is valid and then returns zero.

		 	To get a specific tensor with other components, inherit from tensor
		 	and implement the virtual `Evaluate` method.

		 	Two tensors can be added if the same indices appear. Furthermore
		 	we can multiply a tensor by an arbitrary real number and two tensors
		 	with different indices. Since we deal with only covariant tensors,
		 	we do not implement the Einstein sum convention here. This may
		 	follow in the future.
		 */
		class Tensor : public Printable, Serializable<Tensor> {
		public:
			enum class TensorType {
				ADDITION = 1,
				MULTIPLICATION = 2,
				SCALED = 3,

				SCALAR = 101,
				NUMERIC = 102,

				EPSILON = 201,
				GAMMA = 202,
				EPSILONGAMMA = 203,

				SUBSTITUTE = 301,

				CUSTOM = -1
			};
		public:
			Tensor() : Printable("") { }

			/**
				Constructor of a Tensor

			 	\param name			The name of the tensor
			 	\param printable	The printed text of the tensor
			 	\param indices		The indices of the tensor
			 */
			Tensor(const std::string& name, const std::string& printable, const Indices& indices)
				: name(name), Printable(printable), indices(indices) { }

			// Copy constructor
			Tensor(const Tensor& other)
				: name(other.name), Printable(other.printed_text), indices(other.indices), type(other.type), symmetries(other.symmetries) { }

			// Move constructor
			Tensor(Tensor&& other)
				: name(std::move(other.name)), Printable(std::move(other.printed_text)), indices(std::move(other.indices)), type(std::move(other.type)), symmetries(std::move(other.symmetries)) { }
		public:
			// Copy assignment
			Tensor& operator=(const Tensor& other) {
				name = other.name;
				printed_text = other.printed_text;
				indices = other.indices;
				type = other.type;
				symmetries = other.symmetries;
				return *this;
			}

			// Move assignment
			Tensor& operator=(Tensor&& other) {
				name = std::move(other.name);
				printed_text = std::move(other.printed_text);
				indices = std::move(other.indices);
				type = std::move(other.type);
				symmetries = std::move(other.symmetries);
				return *this;
			}
		public:
			virtual std::shared_ptr<Tensor> Clone() const {
				return std::make_shared<Tensor>(*this);
			}
		public:
			/**
				Check if two tensors are equal

			 	TODO: this is of course not really correct so far
			 		  since two tensors are only equal if all the
			 		  components are equal. Since we do not
			 		  know anything about components so far
			 		  we will change this in the future
			 */
			virtual bool operator==(const Tensor& other) const {
				return name == other.name && printed_text == other.printed_text && indices == other.indices;
			}

			/**
				Check if two tensors are unequal
			 */
			virtual bool operator!=(const Tensor& other) const {
				return name != other.name || printed_text != other.printed_text || indices != other.indices;
			}
		public:
			/**
				Check if two tensors are completely equal, i.e.
			 	the components match
			 */
			bool IsEqual(const Tensor& other) const {
				// If the indices do not match, the tensors are clearly not equal
				if (indices != other.indices) return false;

				// Get all index combinations
				auto combinations = GetAllIndexCombinations();

				// Iterate over all index combinations
				for (auto& combination : combinations) {
					// if the components do not match => return false
					if (Evaluate(combination) != other(combination)) return false;
				}

				// Tensors are equal
				return true;
			}
		public:
			Indices GetIndices() const { return indices; }
			std::string GetName() const { return name; }

			void SetName(const std::string& name) { this->name = name; }
			virtual void SetIndices(const Indices& indices) { this->indices = indices; }
		public:
			void PermuteIndices(const Permutation& permutation) {
				indices = permutation(indices);
			}


		public:
			/**
				\brief LaTeX output of a tensor

			 	For a tensor, the output function is overwritten in order to
			 	also include the indices. Note that so far all indices
			 	are covariant.
			 */
			virtual std::string ToString() const override {
				std::stringstream ss;
				ss << printed_text << "_" << indices;
				return ss.str();
			}
		protected:
			/**
				\brief Check if the index assignment is fine

			 	This method will check the index assignment (in form of
			 	variadic templates) by checking the ranges for each index.
			 	If this fails an exception is thrown.
			 */
			template<typename T, typename... Args>
			std::vector<unsigned> CheckIndices(T t, Args... args) const {
				return indices(t, args...);
			}
		public:
			/**
				Virtual method for tensor evaluation. Standard result is zero.

			 	@DEPRECEATED! Replace with evaluator

			 	\param indices	Vector with the index assigment, i.e unsigned ints
			 	\returns		The tensor component at this index assignment
			 */
			virtual ScalarPointer Evaluate(const std::vector<unsigned>& indices) const {
				return ScalarPointer(new Fraction(0));
			}

			/**
				\brief Syntactic sugar for tensor evaluation.

				Syntactic sugar for tensor evaluation. Allows users to
			 	directly enter the index assignment in the usual syntax.
			 	It will automatically check the indices and then evaluate
			 	the tensor.

			 	For example `T(1,2,3)` will output the 123 component of the
			 	tensor.
			 */
			template<typename T, typename... Args>
			ScalarPointer operator()(T t, Args... args) const {
				auto indices = CheckIndices(t, args...);
				return Evaluate(indices);
			}

			/**
				\brief Evaluate the tensor with an IndexAssignments object

				Evaluate the tensor with an IndexAssignments object.
			 	In this case the values to a specific index are
			 	given as a std::map which allows the assignment to
			 	permutations, added and multiplied tensors.
			 */
			inline ScalarPointer operator()(const IndexAssignments& assignment) const {
				auto result = assignment(indices);
				return Evaluate(result);
			}

			/**
				\brief Alias for index assignment

			 	Alias for index assignment. It simply calls the Evaluate function
			 	of the tensor class.

			 	\param indices	Vector with the index assigment, i.e unsigned ints
			 	\returns		The tensor component at this index assignment
			 */
			inline ScalarPointer operator()(const std::vector<unsigned>& indices) const {
				return Evaluate(indices);
			}
		public:
			/**
				\brief Brings the indices in normal order
			 */
			virtual std::shared_ptr<Tensor> Canonicalize() const {
				return std::make_shared<Tensor>(*this);
			}
        public:
            bool IsCustomTensor() const { return type == TensorType::CUSTOM; }

            bool IsAddedTensor() const { return type == TensorType::ADDITION; }
            bool IsMultipliedTensor() const { return type == TensorType::MULTIPLICATION; }
            bool IsScaledTensor() const { return type == TensorType::SCALED; }

            bool IsScalar() const { return type == TensorType::SCALAR; }
            bool IsNumeric() const { return type == TensorType::NUMERIC; }
			bool IsSubstitute() const { return type == TensorType::SUBSTITUTE; }

            bool IsEpsilonTensor() const { return type == TensorType::EPSILON; }
            bool IsGammaTensor() const { return type == TensorType::GAMMA; }

			std::string TypeToString() const {
				switch (type) {
					case TensorType::ADDITION: return "Addition";
					case TensorType::MULTIPLICATION: return "Multiplication";
					case TensorType::SCALED: return "Scaled";
					case TensorType::SCALAR: return "Scalar";
					case TensorType::SUBSTITUTE: return "Substitute";
					case TensorType::GAMMA: return "Gamma";
					case TensorType::EPSILON: return "Epsilon";
					case TensorType::EPSILONGAMMA: return "EpsilonGamma";
					default: return "Custom";
				}
			}
		public:
			/**
				\brief Multiplication of two tensors

			 	Multiplication of two tensors. It encapsulates the two tensors
			 	by saving smart pointers to them and when evaluating uses
			 	them to generate the result.

			 	Note that an CannotMultiplyTensorsException is thrown if the tensors
			 	have one or more common indices.

			 	\throws CannotMultiplyTensorsException
			 */
			MultipliedTensor operator*(const Tensor& other) const;

			/**
				\brief Multiplication of a tensor by a real number

			 	Multiplication of a tensor by a real number. It stores a
			 	pointer to the tensor and the number and when evaluating uses
			 	them to calculate the result.
			 */
			ScaledTensor operator*(const Scalar& c) const;
			friend inline ScaledTensor operator*(const Scalar& c, const Tensor& other);

			/**
				\brief Negation of a tensor

			 	Negation of a tensor. Is equal to a multiplication by -1.
			 */
			inline ScaledTensor operator-() const;

			/**
				\brief Addition of two tensors

			 	Addition of two tensors. It encapsulates the two tensors
			 	by saving smart pointers to them and when evaluating uses
			 	them to generate the result.

			 	Note that an CannotAddTensorsException is thrown if the tensors
			 	have different indices although the order does not matter.

			 	\throws CannotAddTensorsException
			 */
			AddedTensor operator+(const Tensor& other) const;

			/**
				\brief Subtraction of two tensor

			 	Subtraction of two tensors. Is equal to the sum of the tensor
			 	and the second tensor multiplied by -1.

			 	\throws CannotAddTensorsException
			 */
			AddedTensor operator-(const Tensor& other) const;
		public:
			/**
				\brief Checks if all the ranges are equal

			 	Checks if all the ranges of the indices are equal. If we for example
			 	multiply the Levi-Civita symbol on the spatial slice with the
			 	metric in all spacetime we have two indices that range from
			 	0 to 3 and three from 1 to 3. In this case the function
			 	returns false.
			 */
			bool AllRangesEqual() const {
				for (auto& index : indices) {
					if (index.GetRange() != indices[0].GetRange()) return false;
				}
				return true;
			}

			/**
				\brief Returns all the possible index combinations for the tensor.

			 	Returns all the possible index combinations for the tensor.
			 	It uses a recursive inline methods that fixes the indices one by one
			 	until all indices have a value. In this case the combination is added to the
			 	result.
			 */
			std::vector<std::vector<unsigned>> GetAllIndexCombinations() const {

				// Result
				std::vector<std::vector<unsigned>> result;

				// Helper method to recursively determine the index combinations
				std::function<void(const std::vector<unsigned>&)> fn = [&](const std::vector<unsigned>& input) -> void {
					// If all indices are fixed, add the combination to the list
					if (input.size() == indices.Size()) {
						result.push_back(input);
						return;
					}

					// Get range of next unfixed index
					auto range = indices[input.size()].GetRange();

					// Iterate over the range
					for (auto i : range) {
						// Add the index to the list
						std::vector<unsigned> newInput = input;
						newInput.push_back(i);

						// Recursive call to go to next index
						fn(newInput);
					}
				};

				// Start recursion
				std::vector<unsigned> input;
				fn({});

				return result;
			}

			virtual std::vector<std::vector<unsigned>> GetAllInterestingIndexCombinations() const {
				return GetAllIndexCombinations();
			}

			/**
				\brief Checks if the tensor is identical to zero

			 	Checks if the tensor is identical to zero. For
			 	this it evaluates at all the possible index combinations
				and immediately returns false if one combination does
			 	not yield zero.
			 */
			bool IsZero() const {
				// Get all index combinations
				auto combinations = GetAllIndexCombinations();

				// Iterate over all combinations
				for (auto& combination : combinations) {
					auto r = Evaluate(combination);
					if (r->HasVariables() || r->ToDouble() != 0) return false;
					//if (Evaluate(combination) != 0) return false;
				}

				return true;
			}

			bool IsIndexEqual(const Tensor& other) const {
				if (other.type != type) return false;
				return symmetries.IsEqual(indices, other.indices);
			}

			/*bool IsEqual(const Tensor& tensor) const {
				return (*this - tensor).IsZero();
			}*/
		public:
			void Serialize(std::ostream& os) const override;
			static std::shared_ptr<Tensor> Deserialize(std::istream& is);
		private:
			friend class boost::serialization::access;

			/**
				Serialization of a tensor. Stores the name, LaTeX code and the
			 	indices to an archive
			 */
			template<class Archive>
			void serialize(Archive& ar, const unsigned int version) {
				/*ar & name;
				ar & printed_text;
                ar & type;
				ar & indices;*/
			}
		protected:
			std::string name;
			Indices indices;

			TensorType type;
			Symmetry symmetries;

			//EvaluationFunction evaluator;
		};

		// Syntactic sugar for pointers to tensors
		typedef std::shared_ptr<Tensor> TensorPointer;
		typedef std::shared_ptr<const Tensor> ConstTensorPointer;

		/**
			\class AddedTensor

		 	The sum of two (arbitrary) tensors. This should never be explicitely
			constructed without garantueeing that the indices of both tensors
		    match.
		 */
		class AddedTensor : public Tensor {
		public:
			/**
				Constructor of an AddedTensor
			 */
			AddedTensor(TensorPointer A, TensorPointer B)
				: Tensor("", "", A->GetIndices()), A(A), B(B) {

				type = TensorType::ADDITION;
			}
		public:
			virtual TensorPointer Clone() const override {
				return std::make_shared<AddedTensor>(
					std::move(A->Clone()),
					std::move(B->Clone())
				);
			}
		public:
			/**
				Return the LaTeX code of both tensors
			 */
			virtual std::string ToString() const override;
		public:
			inline TensorPointer GetFirst() const { return A; }
			inline TensorPointer GetSecond() const { return B; }
		public:
			static void DoSerialize(std::ostream& os, const AddedTensor& tensor) {
				tensor.A->Serialize(os);
				tensor.B->Serialize(os);
			}

			static TensorPointer DoDeserialize(std::istream& is, const Indices& indices) {
				auto A = Tensor::Deserialize(is);
				auto B = Tensor::Deserialize(is);
				return std::make_shared<AddedTensor>(std::move(A), std::move(B));
			}
		public:
			/**
				Set the indices to the new order
			 */
			virtual void SetIndices(const Indices& newIndices) override {
				// Need to permute the indices in A and B
				auto permutationA = Permutation::From(indices, A->GetIndices());
				auto permutationB = Permutation::From(indices, B->GetIndices());

				indices = newIndices;
				A->SetIndices(permutationA(newIndices));
				B->SetIndices(permutationB(newIndices));
			}
		public:
			/**
            	\brief Evaluate the components of the sum

                Evaluate the components of the sum. It first checks the
                index assignment and afterwards generates an IndexAssignments
                instance. that is passed to both tensors.

                The reason for the IndexAssignments object is that we need
                terms as
                	 T_{ab} + T_{ba}
                This gives us a tensor with {ab} indices, but the assignment
            	has to incorporate the arrangement of the tensors.

            	\throws IncompleteIndexAssignmentException
             */
			virtual ScalarPointer Evaluate(const std::vector<unsigned>& args) const override {
				// If number of args and indices differ return
				if (args.size() != indices.Size()) {
					throw IncompleteIndexAssignmentException();
				}

				// Create index assignments
				IndexAssignments assignment;
				for (int i=0; i<args.size(); i++) {
					assignment[indices[i].GetName()] = args[i];
				}

				return Scalar::Add(*(*A)(assignment), *(*B)(assignment));
			}

			/**
				Canonicalize a sum of two tensors
			 */
			TensorPointer Canonicalize() const override {
				auto newA = A->Canonicalize();
				auto newB = B->Canonicalize();
				return std::move(std::make_shared<AddedTensor>(newA, newB));
			}
		public:
			template<class Archive>
			void serialize(Archive& ar, const unsigned version) {
				ar & boost::serialization::base_object<Tensor>(*this);
				ar & A;
				ar & B;
			}
		private:
			TensorPointer A;
			TensorPointer B;
		};

		/**
		 	\class MultipliedTensor

		 	\brief Class for two multiplied tensors.

		 	Class for two multiplied tensors. This should never be
		 	created without garantueeing that each index occurs maximally
		 	once or otherwise the tensor cannot properly evaluated.

		 	Mathematically this means that the multiplication of two tensors
		 		S_{abc} T_{de}
		 	gives us a tensor with indices {abcde}
		 */
		class MultipliedTensor : public Tensor {
		public:
			MultipliedTensor(ConstTensorPointer A, ConstTensorPointer B)
				: Tensor("", "", A->GetIndices()), A(A), B(B)
			{
				// Insert the remaining indices of B
				for (auto& index : B->GetIndices()) {
					indices.Insert(index);
				}

				type = TensorType::MULTIPLICATION;
			}
		public:
			virtual TensorPointer Clone() const override {
				return std::make_shared<MultipliedTensor>(
					std::move(A->Clone()),
					std::move(B->Clone())
				);
			}
		public:
			virtual std::string ToString() const override {
				std::stringstream ss;
				ss << A->ToString() << B->ToString();
				return ss.str();
			}

			/**
            	\brief Evaluates the tensor component

            	Evaluates the tensor components. For this, we first
            	check the index assignment, then seperate the
            	assignments into two IndexAssignments objects for each
            	tensor and assign them to the given tensors.

            	\throws IncompleteIndexAssignmentException
         	 */
			virtual ScalarPointer Evaluate(const std::vector<unsigned>& args) const override {
				// If number of args and indices differ return
				if (args.size() != indices.Size()) {
					throw IncompleteIndexAssignmentException();
				}

				// Create index assignments
				IndexAssignments assignment1;
				IndexAssignments assignment2;

				// First N indices belong to A
				for (int i=0; i<A->GetIndices().Size(); i++) {
					assignment1[indices[i].GetName()] = args[i];
				}

				// Remaining indices belong to B
				for (int i=A->GetIndices().Size(); i<args.size(); i++) {
					assignment2[indices[i].GetName()] = args[i];
				}

				return Scalar::Multiply(*(*A)(assignment1), *(*B)(assignment2));
			}
		public:
			static void DoSerialize(std::ostream& os, const MultipliedTensor& tensor) {
				tensor.A->Serialize(os);
				tensor.B->Serialize(os);
			}

			static TensorPointer DoDeserialize(std::istream& is, const Indices& indices) {
				auto A = Tensor::Deserialize(is);
				auto B = Tensor::Deserialize(is);
				return std::make_shared<MultipliedTensor>(std::move(A), std::move(B));
			}

			template<class Archive>
			void serialize(Archive& ar, const unsigned version) {
				ar & boost::serialization::base_object<Tensor>(*this);
				ar & A;
				ar & B;
			}
		private:
			ConstTensorPointer A;
			ConstTensorPointer B;
		};

		/**
		 	\class ScaledTensor

		 	\brief Class for a tensor multiplied by a real number.

		 	Class for a tensor multiplied by a real number. This should never
		 	be created without garantueeing that each index occurs maximally
		 	once or otherwise the tensor cannot properly evaluated.
		 */
		class ScaledTensor : public Tensor {
		public:
			ScaledTensor(ConstTensorPointer A, const Scalar& c)
				: Tensor("", "", A->GetIndices()), A(A), c(c) {

				type = TensorType::SCALED;
			}
		public:
			virtual TensorPointer Clone() const override {
				return std::make_shared<ScaledTensor>(
					std::move(A->Clone()),
					c
				);
			}
		public:
			/**
				\brief Evaluates the tensor component

				Evaluates the tensor components. We can directly
				do so by feeding the arguments to the tensor and
				multiplying the evaluated result with the scale
				factor c

				\throws IncompleteIndexAssignmentException
			 */
			virtual double Evaluate(const std::vector<unsigned>& args) const override {
				return c * A->Evaluate(args);
			}

			TensorPointer Canonicalize() const override {
				auto newA = A->Canonicalize();
				if (newA->IsScaledTensor()) {
					ScaledTensor* scaled = static_cast<ScaledTensor*>(newA.get());
					scaled->SetScale(c * scaled->GetScale());
				} else {
					newA = std::make_shared<ScaledTensor>(std::move(newA), c);
				}
				return std::move(newA);
			}

			virtual std::string ToString() const override {
				std::stringstream ss;

				if (c == 1) {
					// do nothing
				} else if (c == -1) {
					ss << "-";
				} else {
					ss << c << "*";
				}

				// if sum of tensors, place brackets
				if (A->IsAddedTensor())
				ss << "(" << A->ToString() << ")";
				else ss << A->ToString();

				return ss.str();
			}
		public:
			ScaledTensor operator*(double c) const {
				return ScaledTensor(
						A,
						c*this->c
				);
			}

			friend inline ScaledTensor operator*(double c, const ScaledTensor& other) {
				return ScaledTensor(
						other.A,
						c*other.c
				);
			}

			ScaledTensor operator-() const {
				return ScaledTensor(
					A,
					-c
				);
			}
		public:
			ConstTensorPointer GetTensor() const {
				return A;
			}

			double GetScale() const { return c; }
			void SetScale(double c) { this->c = c; }
		public:
			static void DoSerialize(std::ostream& os, const ScaledTensor& tensor) {
				os.write(reinterpret_cast<const char*>(&tensor.c), sizeof(&tensor.c));
				tensor.A->Serialize(os);
			}

			static TensorPointer DoDeserialize(std::istream& is, const Indices& indices) {
				auto A = Tensor::Deserialize(is);

				double c;
				is.read(reinterpret_cast<char*>(&c), sizeof(c));

				return std::make_shared<ScaledTensor>(std::move(A), c);
			}

			template<class Archive>
			void serialize(Archive& ar, const unsigned version) {
				ar & boost::serialization::base_object<Tensor>(*this);
				ar & A;
				ar & c;
			}
		private:
			ConstTensorPointer A;
			double c;
		};

		MultipliedTensor Tensor::operator*(const Tensor &other) const {
			// Check if all indices are distinct
			for (auto& index : indices) {
				if (other.indices.ContainsIndex(index)) {
					throw CannotMultiplyTensorsException();
				}
			}

			return MultipliedTensor(
					std::move(ConstTensorPointer(ConstTensorPointer(), this)),
					std::move(ConstTensorPointer(ConstTensorPointer(), &other))
			);
		}

		ScaledTensor Tensor::operator*(double c) const {
			return ScaledTensor(
				std::move(ConstTensorPointer(ConstTensorPointer(), this)),
				c
			);
		}

		ScaledTensor operator*(double c, const Tensor& other) {
			return other*c;
		}

		inline ScaledTensor Tensor::operator-() const {
			return -1 * (*this);
		}

		AddedTensor Tensor::operator+(const Tensor& other) const {
			// Tensors can only be added if it contains the same indices (but not necessarily in the same slots)
			if (!indices.IsPermutationOf(other.indices)) throw CannotAddTensorsException();
			return AddedTensor(
				std::move(Clone()),
				std::move(other.Clone())
			);
		}

		std::string AddedTensor::ToString() const {
			std::stringstream ss;

			if (B->IsScaledTensor() && static_cast<const ScaledTensor*>(B.get())->GetScale() == -1) {
				ss << A->ToString() << " - " << static_cast<const ScaledTensor*>(B.get())->GetTensor()->ToString();
			} else {
				ss << A->ToString() << " + " << B->ToString();
			}
			return ss.str();
		}

		/**
		 	\class SubstituteTensor

		 	Tensor that only changes the index structure. This is
		 	just a helper class.
		 */
		class SubstituteTensor : public Tensor {
		public:
			SubstituteTensor(TensorPointer A, const Indices& indices) : Tensor("", "", indices), A(A) {
				type = TensorType::SUBSTITUTE;

				if (!indices.IsPermutationOf(A->GetIndices())) {
					throw Exception("The indices have to be a permutation of each other");
				}
			}
		public:
			virtual TensorPointer Clone() const override {
				return std::make_shared<SubstituteTensor>(
						std::move(A->Clone()),
						indices
				);
			}
		public:
			bool IsAddedTensor() const {
				return A->IsAddedTensor();
			}
		public:
			virtual std::string ToString() const override {
				return A->ToString();
			}

			virtual double Evaluate(const std::vector<unsigned>& args) const override {
				// If number of args and indices differ return
				if (args.size() != indices.Size()) {
					throw IncompleteIndexAssignmentException();
				}

				// Create index assignments
				IndexAssignments assignment;
				for (int i=0; i<args.size(); i++) {
					assignment[indices[i].GetName()] = args[i];
				}

				return (*A)(assignment);
			}
		public:
			/**
				Set the indices to the new order
			 */
			virtual void SetIndices(const Indices& newIndices) override {
				// Need to permute the indices in A and B
				auto permutationA = Permutation::From(indices, A->GetIndices());

				indices = newIndices;
				A->SetIndices(permutationA(newIndices));
			}

			/**
				Get the indices of the substituted tensor
			 */
			Indices GetPermutedIndices() const {
				return A->GetIndices();
			}
		public:
			static void DoSerialize(std::ostream& os, const SubstituteTensor& tensor) {
				tensor.A->Serialize(os);
			}

			static TensorPointer DoDeserialize(std::istream& is, const Indices& indices) {
				auto A = Tensor::Deserialize(is);
				return std::make_shared<SubstituteTensor>(std::move(A), indices);
			}
		private:
			TensorPointer A;
		};

		/**
			\class ScalarTensor
		 */
		class ScalarTensor : public Tensor {
		public:
			ScalarTensor(double value) : value(value) { }

			ScalarTensor(const std::string& name, const std::string& printed_text, double value)
				: Tensor(name, printed_text, Indices()), value(value) {

				type = TensorType::SCALAR;
			}
		public:
			virtual TensorPointer Clone() const override {
				return std::make_shared<ScalarTensor>(*this);
			}
		public:
			virtual std::string ToString() const override {
				return printed_text;
			}
        public:
			virtual double Evaluate(const std::vector<unsigned>& args) const override {
				return value;
			}
		public:
			double operator()() const {
				return value;
			}
		public:
			static void DoSerialize(std::ostream& os, const ScalarTensor& tensor) {
				os.write(reinterpret_cast<const char*>(&tensor.value), sizeof(tensor.value));
			}

			static TensorPointer DoDeserialize(std::istream& is, const Indices& indices) {
				double value;
				is.read(reinterpret_cast<char*>(&value), sizeof(value));

				return std::make_shared<ScalarTensor>(value);
			}

			template<class Archive>
			void serialize(Archive& ar, const unsigned version) {
				/*ar & boost::serialization::base_object<Tensor>(*this);
				ar & value;*/
			}
		private:
			double value;
		};

		inline ScalarTensor One() {
			return ScalarTensor("1", "1", 1);
		}

		/**
			\class EpsilonTensor

		 	\brief Represents the totally antisymmetric tensor density

			Represents the totally antisymmetric tensor density also known as
		 	the Levi-Civita symbol. It is defined by
		 							   _
		 							  | +1  {a1...an} is an even permutation of {0...n}
		 		\epsilon_{a1...an} =  | -1  {a1...an} is an odd permutation of {0...n}
		 		                      |_ 0  else
		 */
		class EpsilonTensor : public Tensor {
		public:
			EpsilonTensor(const Indices& indices)
				: Tensor("epsilon", "\\epsilon", indices) {

				assert(indices[0].GetRange().GetTo()+1 -indices[0].GetRange().GetFrom() == indices.Size());
				type = TensorType::EPSILON;
				symmetries.Add({ {0,1,2} , false});
			}
		public:
			virtual TensorPointer Clone() const override {
				return std::make_shared<EpsilonTensor>(*this);
			}
		public:
			/**
			    \brief Evaluate the Levi-Civita symbol

			 	Evaluate the Levi-Civita symbol. For this we employ
			 	the representation given by

				\epsilon_{a_1...a_n} = \Pi_{1\le p < q \le n} \frac{a_p-a_q}{p-q}

				This is easier to calculate since no permutation have to
				be generated.
			 */
			static double GetEpsilonComponents(const std::vector<unsigned>& args) {
				double result = 1.0;
				for (unsigned p=0; p < args.size(); p++) {
					for (unsigned q=p+1; q < args.size(); q++ ) {
						result *= static_cast<double>(static_cast<int>(args[q])-static_cast<int>(args[p]))/(q-p);
					}
				}
				return result != 0 ? result : 0;
			}


			virtual double Evaluate(const std::vector<unsigned>& args) const override {
				return GetEpsilonComponents(args);
            }

			virtual TensorPointer Canonicalize() const override {
				int sign = 1;

				// Sort indices
				auto sortedIndices = indices.Ordered();

				// Find the sign
				sign = Permutation::From(indices, sortedIndices).Sign();

				// Construct and return result
				if (sign < 0) {
					return std::move(std::make_shared<ScaledTensor>(std::make_shared<EpsilonTensor>(sortedIndices), -1));
				} else {
					return std::move(std::make_shared<EpsilonTensor>(sortedIndices));
				}
			}

			/**
				Returns the Levi-Civita symbol in 3+1 dim spacetime, where
			 	the zeroth index is in temporal direction.
			 */
			static EpsilonTensor SpaceTime(int offset=0) {
				return EpsilonTensor(Indices::GetGreekSeries(4, {0,3}, offset));
			}

			/**
				Returns the Levi-Civita symbol on a spatial slice, i.e.
			 	in 3 dimensions.
			 */
			static EpsilonTensor Space(int offset=0) {
				return EpsilonTensor(Indices::GetRomanSeries(3, {1,3}, offset));
			}
		};

		// Alias
		typedef EpsilonTensor	LeviCivitaTensor;

		/**
			\class GammaTensor

		 	\brief Class for a flat space metric with (p,q) signature

		 	Class for a flat space metric. It has a (p,q) signature
		 */
		class GammaTensor : public Tensor {
		public:
			GammaTensor()
				: Tensor("gamma", "\\gamma", Indices::GetRomanSeries(2, {1,3})), signature({0,3}) {

                type = TensorType::GAMMA;
				std::vector<unsigned> _indices = {0,1};
				symmetries.Add(ElementarySymmetry(_indices));
            }

			GammaTensor(const Indices& indices, int p, int q)
				: Tensor("gamma", "\\gamma", indices), signature({p,q}) {
				assert(indices.Size() == 2);

                type = TensorType::GAMMA;
				std::vector<unsigned> _indices = {0,1};
				symmetries.Add(ElementarySymmetry(_indices));
			}

			GammaTensor(const Indices& indices)
				: Tensor("gamma", "\\gamma", indices),  signature({0,3}) {
				assert(indices.Size() == 2);

                type = TensorType::GAMMA;
				std::vector<unsigned> _indices = {0,1};
				symmetries.Add(ElementarySymmetry(_indices));
			}
		public:
			virtual TensorPointer Clone() const override {
				return std::make_shared<GammaTensor>(*this);
			}
		public:
			std::pair<int, int> GetSignature() const { return signature; }
			void SetSignature(int p, int q) { signature = {p,q}; }
		public:
			/**
				Create a Euclidean metric, i.e. with signature (0,4)
			 */
			static GammaTensor EuclideanMetric(int offset=0) {
				return GammaTensor(Indices::GetGreekSeries(2, {0,3}, offset), 0,4);
			}

			/**
				Create a Minkowskian metric, i.e. with signature (1,3)
			 */
			static GammaTensor MinkowskianMetric(int offset=0) {
				return GammaTensor(Indices::GetGreekSeries(2, {0,3}, offset), 1,3);
			}

			/**
				Create a spatial metric, i.e. with signature (0,3)
			 */
			static GammaTensor SpatialMetric(int offset=0) {
				return GammaTensor(Indices::GetRomanSeries(2, {1,3}, offset), 0,3);
			}
		public:
			/**
				Evaluate the tensor components. It returns 0 if evaluated
			 	off diagonal, -1 for all indices < p and 1 else.
			 */
			virtual double Evaluate(const std::vector<unsigned>& vec) const override {
				if (vec.size() != 2) {
					throw IncompleteIndexAssignmentException();
				}

				if (vec[0] == vec[1]) {
					if (vec[0]-indices[0].GetRange().GetFrom() < signature.first) return -1;
					else return 1;
				}
				return 0;
			}

			virtual TensorPointer Canonicalize() const override {
				auto sortedIndices = indices.Ordered();
				return std::move(std::make_shared<GammaTensor>(sortedIndices, signature.first, signature.second));
			}
		public:
			static void DoSerialize(std::ostream& os, const GammaTensor& tensor) {
				int p = tensor.signature.first;
				int q = tensor.signature.second;

				os.write(reinterpret_cast<const char*>(&p), sizeof(p));
				os.write(reinterpret_cast<const char*>(&q), sizeof(q));
			}

			static TensorPointer DoDeserialize(std::istream& is, const Indices& indices) {
				int p, q;
				is.read(reinterpret_cast<char*>(&p), sizeof(p));
				is.read(reinterpret_cast<char*>(&q), sizeof(q));

				return std::make_shared<GammaTensor>(indices, p, q);
			}

			template<class Archive>
			void serialize(Archive& ar, const unsigned version) {
				/*ar & boost::serialization::base_object<Tensor>(*this);
				ar & signature;*/
			}
		private:
			std::pair<int, int> signature;
		};

		// Alias
		typedef GammaTensor		MetricTensor;

		/**
			\class EpsilonGammaTensor

		 	Class to represent a multiplied version of epsilon and gamma
		 	tensors. Although it is possible to also obtain this with the
		 	help of EpsilonTensor and GammaTensor by multiplication,
		 	this allows us to directly put some optimizations in.

		 	For example we can immediately return the result in case
		 	the epsilon or one of the gammas vanish. It remains to be checked
		 	if a check of the indices will simplify the evaluation.
		 */
		class EpsilonGammaTensor : public Tensor {
		public:
			EpsilonGammaTensor(unsigned numEpsilon, unsigned numGamma, const Indices& indices) : /*Tensor("EpsilonGamma", "\\epsilon\\gamma", indices)*/ Tensor("", "", indices), numEpsilon(numEpsilon), numGamma(numGamma) {
				assert(numEpsilon * 3 + numGamma*2 == indices.Size());

				type = TensorType::EPSILONGAMMA;

				// Introduce all the symmetries
				unsigned i=0;
				std::vector<std::pair<unsigned, unsigned>> blocks;

				if (numEpsilon == 1) {
					std::vector<unsigned> _indices = {0,1,2};
					symmetries.Add(ElementarySymmetry(_indices, false));
					i = 3;
				}
				for (int j=0; j<numGamma; j++) {
					std::vector<unsigned> _indices = {i, i+1};
					symmetries.Add(ElementarySymmetry(_indices));
					blocks.push_back({i, i+1});
					i += 2;
				}

				// Do not forget the block symmetry of the gammas
				symmetries.Add({ blocks });
			}

			EpsilonGammaTensor(const EpsilonGammaTensor& other)
					: Tensor(other), numEpsilon(other.numEpsilon), numGamma(other.numGamma) { }
			EpsilonGammaTensor(EpsilonGammaTensor&& other)
					: Tensor(std::move(other)), numEpsilon(std::move(other.numEpsilon)), numGamma(std::move(other.numGamma)) { }
		public:
			EpsilonGammaTensor& operator=(const EpsilonGammaTensor& other) {
				*this = other;
				numEpsilon = other.numEpsilon;
				numGamma = other.numGamma;
				return *this;
			}

			EpsilonGammaTensor& operator=(EpsilonGammaTensor&& other) {
				*this = std::move(other);
				numEpsilon = std::move(other.numEpsilon);
				numGamma = std::move(other.numGamma);
				return *this;
			}
		public:
			virtual TensorPointer Clone() const override {
				return std::make_shared<EpsilonGammaTensor> (*this);
			}
		public:
			virtual std::string ToString() const override {
				std::stringstream ss;
				unsigned pos = 0;

				if (numEpsilon == 1) {
					ss << "\\epsilon_" << indices.Partial({0,2});
					pos += 3;
				}

				for (unsigned i=0; i<numGamma; i++) {
					ss << "\\gamma_" << indices.Partial({pos, pos+1});
					pos += 2;
				}

				return ss.str();
			}
		public:
			unsigned GetNumEpsilons() const { return numEpsilon; }
			unsigned GetNumGammas() const { return numGamma; }
		public:
			static std::vector<unsigned> Partial(const std::vector<unsigned>& args, Range range) {
				std::vector<unsigned> result;
				for (auto i : range) {
					result.push_back(args[i]);
				}
				return result;
			}

			/*std::vector<std::vector<unsigned>> GetAllInterestingIndexCombinations() const {
				// Result
				std::vector<std::vector<unsigned>> result;

				// Helper method to recursively determine the index combinations
				std::function<void(const std::vector<unsigned>&)> fn = [&](const std::vector<unsigned>& input) -> void {
					// If all indices are fixed, add the combination to the list
					if (input.size() == indices.Size()) {
						result.push_back(input);
						return;
					}

					// Get range of next unfixed index
					auto range = indices[input.size()].GetRange();

					// Iterate over the range
					for (auto i : range) {
						// Add the index to the list
						std::vector<unsigned> newInput = input;
						newInput.push_back(i);

						// Recursive call to go to next index
						fn(newInput);
					}
				};

				// Start recursion
				std::vector<unsigned> input;
				fn({});

				return result;
			}*/

			virtual double Evaluate(const std::vector<unsigned>& args) const override {
				double result = 1.0;
				unsigned pos = 0;

				// Calculate the epsilon contribution
				if (numEpsilon == 1) {
					auto indices = this->indices.Partial({0,2});
					auto partialArgs = Partial(args, {0,2});

					result = EpsilonTensor::GetEpsilonComponents(partialArgs);

					if (result == 0.0) return result;

					pos += 3;
				}

				// Calculate the gamma contribution
				for (unsigned i=0; i<numGamma; i++) {
					auto indices = this->indices.Partial({pos, pos+1});
					auto partialArgs = Partial(args, {pos, pos+1});
					result *= GammaTensor(indices, 0,3).Evaluate(partialArgs);// GammaTensor::Evaluate(partialArgs, GammaTensor(indices, 0,3));

					if (result == 0.0) return result;

					pos += 2;
				}

				return result;
			}

			virtual TensorPointer Canonicalize() const override {
				unsigned pos = 0;
				int sign = 1;

				Indices newIndices;

				// Canonicalize the epsilon contribution
				if (numEpsilon == 1) {
					auto epsilonIndices = indices.Partial({0, 2});

					// Sort and append the indices to the new list
					auto sortedIndices = epsilonIndices.Ordered();
					newIndices.Append(sortedIndices);

					// Find the sign
					sign = Permutation::From(epsilonIndices, sortedIndices).Sign();

					pos += 3;
				}

				// Vector for the sorted indices of the gammas
				std::vector<Indices> gammas;

				// Canonicalize the gamma contribution
				for (unsigned i=0; i<numGamma; i++) {
					auto gammaIndices = indices.Partial({pos, pos+1});

					// Sort and append the indices to the new list
					auto sortedIndices = gammaIndices.Ordered();
					gammas.push_back(sortedIndices);

					pos += 2;
				}

				// Sort the indices of the gammas to respect the
				// commutivity of gammas
				std::sort(gammas.begin(), gammas.end(), [](const Indices& a, const Indices& b) {
					return a[0] < b[0];
				});

				// Append the now sorted gammas to all indices
				for (auto& gammaIndices : gammas) {
					newIndices.Append(gammaIndices);
				}

				// Construct and return result
				if (sign < 0) {
					return std::move(std::make_shared<ScaledTensor>(std::make_shared<EpsilonGammaTensor>(numEpsilon, numGamma, newIndices), -1));
				} else {
					return std::move(std::make_shared<EpsilonGammaTensor>(numEpsilon, numGamma, newIndices));
				}
			}
		public:
			friend class boost::serialization::access;

			static void DoSerialize(std::ostream& os, const EpsilonGammaTensor& tensor) {
				/*unsigned numEpsilon = tensor.numEpsilon;
				unsigned numGamma = tensor.numGamma;

				os.write(reinterpret_cast<const char*>(&tensor.numEpsilon), sizeof(tensor.numEpsilon));
				os.write(reinterpret_cast<const char*>(&tensor.numGamma), sizeof(tensor.numGamma));*/
			}

			static TensorPointer DoDeserialize(std::istream& is, const Indices& indices) {
				unsigned numEpsilon = (indices.Size() % 2 == 0) ? 0 : 1;
				unsigned numGamma = (indices.Size() % 2 == 0) ? indices.Size()/2 : (indices.Size()-3)/2;
				return std::make_shared<EpsilonGammaTensor>(numEpsilon, numGamma, indices);
			}

			template<class Archive>
			void serialize(Archive& ar, const unsigned version) {
				/*ar & boost::serialization::base_object<Tensor>(*this);

				ar & numEpsilon;
				ar & numGamma;*/
			}
		private:
			unsigned numEpsilon;
			unsigned numGamma;
		};







		void Tensor::Serialize(std::ostream& os) const {
			// Serialize name and printed_text
			os << name << ";" << printed_text << ";";

			// Serialize the indices
			indices.Serialize(os);

			// Write type
			int typeC = static_cast<int>(type);
			os.write(reinterpret_cast<const char*>(&typeC), sizeof(typeC));

			switch (type) {
				case Tensor::TensorType::ADDITION:
					AddedTensor::DoSerialize(os, *static_cast<const AddedTensor*>(this));
					break;
				case Tensor::TensorType::MULTIPLICATION:
					MultipliedTensor::DoSerialize(os, *static_cast<const MultipliedTensor*>(this));
					break;
				case Tensor::TensorType::SCALED:
					ScaledTensor::DoSerialize(os, *static_cast<const ScaledTensor*>(this));
					break;
				case Tensor::TensorType::SUBSTITUTE:
					SubstituteTensor::DoSerialize(os, *static_cast<const SubstituteTensor*>(this));
					break;
				case Tensor::TensorType::SCALAR:
					ScalarTensor::DoSerialize(os, *static_cast<const ScalarTensor*>(this));
					break;
				case Tensor::TensorType::EPSILONGAMMA:
					EpsilonGammaTensor::DoSerialize(os, *static_cast<const EpsilonGammaTensor*>(this));
					break;
				case Tensor::TensorType::GAMMA:
					GammaTensor::DoSerialize(os, *static_cast<const GammaTensor*>(this));
					break;

				default: break;
			}
		}

		TensorPointer Tensor::Deserialize(std::istream& is) {
			// Read name
			std::string name;
			std::getline(is, name, ';');

			// Read printed text
			std::string printed_text;
			std::getline(is, printed_text, ';');

			// Read indices
			auto indices = *Indices::Deserialize(is);

			// Read type
			int typeC;
			is.read(reinterpret_cast<char*>(&typeC), sizeof(typeC));
			TensorType type = static_cast<TensorType>(typeC);

			TensorPointer result;

			// If not a standard tensor, do the specific serialization stuff
			switch (type) {
				case TensorType::ADDITION:
					result = std::move(AddedTensor::DoDeserialize(is, indices));
					break;

				case TensorType::MULTIPLICATION:
					result = std::move(MultipliedTensor::DoDeserialize(is, indices));
					break;

				case TensorType::SCALED:
					result = std::move(ScaledTensor::DoDeserialize(is, indices));
					break;

				case TensorType::SCALAR:
					result = std::move(ScalarTensor::DoDeserialize(is, indices));
					break;

				case TensorType::GAMMA:
					result = std::move(GammaTensor::DoDeserialize(is, indices));
					break;

				case TensorType::EPSILONGAMMA:
					result = std::move(EpsilonGammaTensor::DoDeserialize(is, indices));
					break;

				case TensorType::SUBSTITUTE:
					result = std::move(SubstituteTensor::DoDeserialize(is, indices));
					break;

				default:
					result = std::make_shared<Tensor>(name, printed_text, indices);
					break;
			}

			// Assign the name and printed text
			result->SetName(name);
			result->SetPrintedText(printed_text);

			return std::move(result);
		}

	}
}
