#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <tensor/permutation.hpp>

namespace Albus {
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
		class Tensor : public Printable {
		public:
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
				: name(other.name), Printable(other.printed_text), indices(other.indices) { }
			// Move constructor
			Tensor(Tensor&& other)
				: name(std::move(other.name)), Printable(std::move(other.printed_text)), indices(std::move(other.indices)) { }
		public:
			// Copy assignment
			Tensor& operator=(const Tensor& other) {
				name = other.name;
				printed_text = other.printed_text;
				indices = other.indices;
				return *this;
			}

			// Move assignment
			Tensor& operator=(Tensor&& other) {
				name = std::move(other.name);
				printed_text = std::move(other.printed_text);
				indices = std::move(other.indices);
				return *this;
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
			bool operator==(const Tensor& other) const {
				return name == other.name && printed_text == other.printed_text && indices == other.indices;
			}

			/**
				Check if two tensors are unequal
			 */
			bool operator!=(const Tensor& other) const {
				return name != other.name || printed_text != other.printed_text || indices != other.indices;
			}
		public:
			Indices GetIndices() const { return indices; }
			std::string GetName() const { return name; }

			void SetName(const std::string& name) { this->name = name; }
			void SetIndices(const Indices& indices) { this->indices = indices; }
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
			virtual std::string ToString() const {
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

			 	\param indices	Vector with the index assigment, i.e unsigned ints
			 	\returns		The tensor component at this index assignment
			 */
			virtual double Evaluate(const std::vector<unsigned>& indices) const {
				return 0.0;
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
			double operator()(T t, Args... args) const {
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
			inline double operator()(const IndexAssignments& assignment) const {
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
			inline double operator()(const std::vector<unsigned>& indices) const {
				return Evaluate(indices);
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
			ScaledTensor operator*(double c) const;
			friend inline ScaledTensor operator*(double c, const Tensor& other);

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
				fn({});

				return result;
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
					if (Evaluate(combination)) return false;
				}

				return true;
			}

			/*bool IsEqual(const Tensor& tensor) const {
				return (*this - tensor).IsZero();
			}*/
		private:
			friend class boost::serialization::access;

			/**
				Serialization of a tensor. Stores the name, LaTeX code and the
			 	indices to an archive
			 */
			template<class Archive>
			void serialize(Archive& ar, const unsigned int version) {
				ar & name;
				ar & printed_text;
				ar & indices;
			}
		protected:
			std::string name;
			Indices indices;
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
			AddedTensor(ConstTensorPointer A, ConstTensorPointer B)
				: Tensor("", "", A->GetIndices()), A(A), B(B) { }
		public:
			/**
				Return the LaTeX code of both tensors
			 */
			virtual std::string ToString() const {
				std::stringstream ss;
				ss << A->ToString() << " + " << B->ToString();
				return ss.str();
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
			virtual double Evaluate(const std::vector<unsigned>& args) const {
				// If number of args and indices differ return
				if (args.size() != indices.Size()) {
					throw IncompleteIndexAssignmentException();
				}

				// Create index assignments
				IndexAssignments assignment;
				for (int i=0; i<args.size(); i++) {
					assignment[indices[i].GetName()] = args[i];
				}

				return (*A)(assignment) + (*B)(assignment);
			}
		private:
			ConstTensorPointer A;
			ConstTensorPointer B;
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
			}
		public:
			virtual std::string ToString() const {
				std::stringstream ss;
				ss << A->ToString() << B->ToString();
				return ss.str();
			}
		public:
			/**
				\brief Evaluates the tensor component

			 	Evaluates the tensor components. For this, we first
			 	check the index assignment, then seperate the
			 	assignments into two IndexAssignments objects for each
			 	tensor and assign them to the given tensors.

			 	\throws IncompleteIndexAssignmentException
			 */
			virtual double Evaluate(const std::vector<unsigned>& args) const {
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

				return (*A)(assignment1) * (*B)(assignment2);
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
			ScaledTensor(ConstTensorPointer A, double c)
				: Tensor("", "", A->GetIndices()), A(A), c(c) { }
		public:
			virtual std::string ToString() const {
				std::stringstream ss;
				ss << c << "*" << A->ToString();
				return ss.str();
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
			virtual double Evaluate(const std::vector<unsigned>& args) const {
				return c * A->Evaluate(args);
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
				std::move(ConstTensorPointer(ConstTensorPointer(), this)),
				std::move(ConstTensorPointer(ConstTensorPointer(), &other))
			);
		}

		/**
			\class ScalarTensor
		 */
		class ScalarTensor : public Tensor {
		public:
			ScalarTensor(const std::string& name, const std::string& printed_text, double value)
				: Tensor(name, printed_text, Indices()), value(value) { }
		public:
			virtual std::string ToString() const {
				return printed_text;
			}
		public:
			double operator()() const {
				return value;
			}

			double Evaluate(const std::vector<unsigned>& args) {
				if (args.size() != 0) throw IncompleteIndexAssignmentException();
				return value;
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
			}
		public:
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
		public:
			/**
				\brief Evaluate the Levi-Civita symbol

			 	Evaluate the Levi-Civita symbol. For this we employ
			 	the representation given by

					\epsilon_{a_1...a_n} = \Pi_{1\le p < q \le n} \frac{a_p-a_q}{p-q}

			 	This is easier to calculate since no permutation have to
			 	be generated.
			 */
			virtual double Evaluate(const std::vector<unsigned>& vec) const {
				double result = 1.0;
				for (unsigned p=0; p < vec.size(); p++) {
					for (unsigned q=p+1; q < vec.size(); q++ ) {
						result *= static_cast<double>(static_cast<int>(vec[q])-static_cast<int>(vec[p]))/(q-p);
					}
				}
				return result != 0 ? result : 0;
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
				: Tensor("gamma", "\\gamma", Indices::GetRomanSeries(2, {1,3})), signature({0,3}) { }

			GammaTensor(const Indices& indices, int p, int q)
				: Tensor("gamma", "\\gamma", indices), signature({p,q}) {
				assert(indices.Size() == 2);
			}

			GammaTensor(const Indices& indices)
				: Tensor("gamma", "\\gamma", indices),  signature({0,3}) {
				assert(indices.Size() == 2);
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
			virtual double Evaluate(const std::vector<unsigned>& vec) const {
				if (vec.size() != 2) {
					throw IncompleteIndexAssignmentException();
				}

				if (vec[0] == vec[1]) {
					 if (vec[0]-indices[0].GetRange().GetFrom() < signature.first) return -1;
					 else return 1;
				}
				return 0;
			}
		private:
			std::pair<int, int> signature;
		};

		// Alias
		typedef GammaTensor		MetricTensor;

	}
}