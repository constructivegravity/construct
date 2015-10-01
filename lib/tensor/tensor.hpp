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

		class AddedTensor;
		class MultipliedTensor;
	
		class Tensor : public Printable {
		public:
			Tensor(const std::string& name, const std::string& printable, const Indices& index) : name(name), Printable(printable), index(index) { }
		public:
			Indices GetIndices() const { return index; }
			std::string GetName() const { return name; }
		public:
			virtual std::string ToString() const {
				std::stringstream ss;
				ss << printed_text << "_" << index;
				return ss.str();
			}
		protected:
			template<typename T, typename... Args>
			std::vector<unsigned> CheckIndices(T t, Args... args) {
				return index(t, args...);
			}
		public:
			template<typename T, typename... Args>
			double operator()(T t, Args... args) {
				auto indices = CheckIndices(t, args...);
				return 0.0;
			}
			
			MultipliedTensor operator*(const Tensor& other) const;

			AddedTensor operator+(const Tensor& other) const;
		private:
			friend class boost::serialization::access;

			template<class Archive>
			void serialize(Archive& ar, const unsigned int version) {
				ar & name;
				ar & printed_text;
				ar & index;
			}
		protected:
			std::string name;
			Indices index;
		};

		class AddedTensor : public Tensor {

		};
		
		class MultipliedTensor : public Tensor {
		public:
			
		};

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

			 	TODO: Note that there is still some bug in this, so do not use this
			 */
			static EpsilonTensor InSpaceTime() {
				return EpsilonTensor(Indices::GetGreekSeries(4, {0,3}));
			}

			static EpsilonTensor InSpace() {
				return EpsilonTensor(Indices::GetGreekSeries(3, {1,3}));
			}
		public:
			template<typename T, typename... Args>
			double operator()(T t, Args... args) {
				// Check indices
				auto indices = CheckIndices(t, args...);

				// Convert index combinations into vector
				std::vector<int> vec = { t, args... };

				double result = 1.0;
				for (int p=0; p < vec.size(); p++) {
					for (int q=p+1; q < vec.size(); q++ ) {
						result *= static_cast<double>(vec[p]-vec[q])/(p-q);
					}
				}
				return (result != 0) ? result : 0;

				// Create standard combination
				/**std::vector<int> standard (indices.size());
				std::iota(standard.begin(), standard.end(), this->index[0].GetRange().GetFrom());

				try {
					auto permutation = Permutation::From(vec, standard);
					return permutation.Sign();
				} catch (IsNoPermutationException e) {
					return 0;
				}*/
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
			template<typename T>
			double operator()(T s, T t) {
				// Check indices
				auto indices = CheckIndices(s, t);

				if (s == t) return 1;
				return 0;
			}
		};

		MultipliedTensor Tensor::operator*(const Tensor &other) const {

		}

		AddedTensor Tensor::operator+(const Tensor &other) const {
			// Tensors can only be added if it contains the same indices (but not necessarily in the same slots)
			assert(index.IsPermutationOf(other.index));
		}
	
	}
}