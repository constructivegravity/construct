#pragma once

#include <cassert>

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>

namespace Albus {
	namespace Tensor {
		
		class BinaryPermutation {
		public:
			BinaryPermutation() = default;
			
			BinaryPermutation(unsigned a, unsigned b) : a(a), b(b) { }
			BinaryPermutation(const BinaryPermutation& other)
				: a(other.a), b(other.b) { }
		public:
			bool operator==(const BinaryPermutation& other) const {
				return a == other.a && b == other.b;
			}
			
			bool operator!=(const BinaryPermutation& other) const {
				return a != other.a || b != other.b;
			}
		public:
			Indices operator()(const Indices& indices) const {
				assert(a > 0 && b > 0 && indices.Size() >= a && indices.Size() >= b);
				
				Indices result;
				
				for (unsigned i=0; i < indices.Size(); ++i) {
					if (i == a-1) result.Insert(indices[b-1]);
					else if (i == b-1) result.Insert(indices[a-1]);
					else result.Insert(indices[i]);
				}
					
				return result;
			}
			
			std::vector<int> operator()(std::vector<int> list) const {
				assert(a > 0 && b > 0 && list.size() >= a && list.size() >= b);
				
				std::vector<int> result;
				
				for (unsigned i=0; i < list.size(); ++i) {
					if (i == a-1) result.push_back(list[b-1]);
					else if (i == b-1) result.push_back(list[a-1]);
					else result.push_back(list[i]);
				}
					
				return result;
			}
			
			template<typename... Args>
			std::vector<int> operator()(Args... args) const {
				return (*this)({ args... });
			}
			
			/*Tensor operator()(const Tensor& tensor) const {
				return Tensor
			}*/
		private:
			unsigned a;
			unsigned b;
		};
		
		/**
			\class Permutation
		
			Allows arbitrary permutation of an index combination
		 */
		class Permutation {
		public:
			Permutation() = default;
			
			Permutation(unsigned a, unsigned b) {
				this->permute.emplace_back(BinaryPermutation(a,b));
			}
			
			Permutation(const BinaryPermutation& permute) {
				this->permute.push_back(permute);
			}
			
			Permutation(std::vector<BinaryPermutation> permute)
				: permute(permute) { }
			
			// Copy constructor
			Permutation(const Permutation& other)
				: permute(other.permute) { }
		public:
			/*Permutation& operator=(std::vector<BinaryPermutation> permute) {
				this->permute = permute;
				return *this;
			}*/
			
			Permutation& operator=(const Permutation& other) {
				permute = other.permute;
				return *this;
			}
			
			Permutation& operator=(Permutation&& other) {
				permute = std::move(other.permute);
				return *this;
			}
		public:
			void Insert(const BinaryPermutation& p) {
				permute.push_back(p); 
			}
			
			void Insert(unsigned a, unsigned b) {
				permute.emplace_back(BinaryPermutation(a,b));
			}
 		public:
			Indices operator()(const Indices& indices) const {
				Indices result = indices;
				for (auto& p : permute) {
					result = p(result);
				}
				return result;
			}
			
			std::vector<int> operator()(const std::vector<int>& indices) const {
				std::vector<int> result = indices;
				for (auto& p : permute) {
					result = p(result);
				}
				return result;
			}
			
			template<typename... Args>
			std::vector<unsigned> operator()(Args... args) const {
				return (*this)({args...});
			}
		public:
			bool IsEven() const {
				return permute.size() % 2 == 0;
			}
			
			bool IsOdd() const {
				return permute.size() % 2 != 0;
			}
			
			int Sign() const {
				return IsOdd() ? -1 : 1;
			}
		public:
			/**
				\brief A cyclic permutation of the indices
			
				{abcd} => {bcda}
			 */
			static Indices Cyclic(const Indices& indices) {
				Permutation p;
				for (int i=1; i<indices.Size(); i++) {
					p.Insert(i, i+1);
				}
				return p(indices);
			}
			
			static std::vector<int> Cyclic(const std::vector<int>& indices) {
				Permutation p;
				for (int i=1; i<indices.size(); i++) {
					p.Insert(i, i+1);
				}
				return p(indices);
			}
			
			template<typename... Args>
			static std::vector<int> Cyclic(Args... args) {
				return Cyclic({ args... });
			}
			
			static bool IsPermutationOfRange(const Range& range, const std::vector<int>& indices) {
				
			}
		private:
			std::vector<BinaryPermutation> permute;
		};
		
	}
}