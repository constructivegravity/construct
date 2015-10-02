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

		// Forward declaration
		class ScaledTensor;
		class AddedTensor;
		class MultipliedTensor;

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
			virtual std::string ToString() const {
				std::stringstream ss;
				ss << printed_text << "_" << indices;
				return ss.str();
			}
		protected:
			template<typename T, typename... Args>
			std::vector<unsigned> CheckIndices(T t, Args... args) const {
				return indices(t, args...);
			}
		public:
			virtual double Evaluate(const std::vector<unsigned>& indices) const {
				return 0.0;
			}

			template<typename T, typename... Args>
			double operator()(T t, Args... args) const {
				auto indices = CheckIndices(t, args...);
				return Evaluate(indices);
			}

			double operator()(const IndexAssignments& assignment) const {
				auto result = assignment(indices);
				return (*this)(result);
			}

			double operator()(const std::vector<unsigned>& indices) const {
				return Evaluate(indices);
			}
		public:
			MultipliedTensor operator*(const Tensor& other) const;
			ScaledTensor operator*(double c) const;
			friend inline ScaledTensor operator*(double c, const Tensor& other);
			AddedTensor operator+(const Tensor& other) const;
		private:
			friend class boost::serialization::access;

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

		typedef std::shared_ptr<Tensor> TensorPointer;
		typedef std::shared_ptr<const Tensor> ConstTensorPointer;

		class AddedTensor : public Tensor {
		public:
			AddedTensor(ConstTensorPointer A, ConstTensorPointer B)
				: Tensor(A->GetName(), "", A->GetIndices()), A(A), B(B) { }
		public:
			virtual std::string ToString() const {
				std::stringstream ss;
				ss << A->ToString() << " + " << B->ToString();
				return ss.str();
			}
		public:
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

		class MultipliedTensor : public Tensor {
		public:

		};

		class ScaledTensor : public Tensor {
		public:
			ScaledTensor(const Tensor* A, double c)
				: Tensor(A->GetName(), "", A->GetIndices()), A(A), c(c) { }
		public:
			virtual std::string ToString() const {
				std::stringstream ss;
				ss << c << "*" << A->ToString();
				return ss.str();
			}
		public:
			virtual double Evaluate(const std::vector<unsigned>& args) const {
				return c * (*A)(args);
			}
		private:
			const Tensor* A;
			double c;
		};

		class CannotAddTensorsException : public Exception {
		public:
			CannotAddTensorsException() : Exception("Cannot add tensors due to incompatible indices") { }
		};

		MultipliedTensor Tensor::operator*(const Tensor &other) const {

		}

		ScaledTensor Tensor::operator*(double c) const {
			return ScaledTensor(this, c);
		}

		ScaledTensor operator*(double c, const Tensor& other) {
			return other*c;
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
			\class EpsilonTensor

		 	\brief Represents the totally antisymmetric tensor density


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
			static EpsilonTensor InSpaceTime() {
				return EpsilonTensor(Indices::GetGreekSeries(4, {0,3}));
			}

			static EpsilonTensor InSpace() {
				return EpsilonTensor(Indices::GetGreekSeries(3, {1,3}));
			}
		public:
			virtual double Evaluate(const std::vector<unsigned>& vec) const {
				double result = 1.0;
				for (int p=0; p < vec.size(); p++) {
					for (int q=p+1; q < vec.size(); q++ ) {
						result *= static_cast<double>(vec[p]-vec[q])/(p-q);
					}
				}
				return (result != 0) ? result : 0;
			}
		};

		class GammaTensor : public Tensor {
		public:
			GammaTensor()
				: Tensor("gamma", "\\gamma", Indices::GetRomanSeries(2, {1,3})) { }

			GammaTensor(const Indices& indices)
				: Tensor("gamma", "\\epsilon", indices) {
				assert(indices.Size() == 2);
			}
		public:
			virtual double Evaluate(const std::vector<unsigned>& vec) const {
				// TODO: proper exception
				if (vec.size() != 2) return 0.0;

				if (vec[0] == vec[1]) return 1;
				return 0;
			}
		};

	}
}