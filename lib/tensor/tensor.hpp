#pragma once

#include <string>

namespace Albus {
	namespace Tensor {
	
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
			Indices CheckIndices(T t, Args... args) {
				return index(t, args...);
			}
		public:
			template<typename T, typename... Args>
			double operator()(T t, Args... args) {
				auto indices = CheckIndices(t, args...);
				return 0.0;
			}
		private:
			std::string name;
			Indices index;
		};
		
		class EpsilonTensor : public Tensor {
		public:
			/*EpsilonTensor() 
				: Tensor("epsilon", "\\epsilon", ) { }*/
		public:
			template<typename T, typename... Args>
			double operator()(T t, Args... args) {
				auto indices = CheckIndices(t, args...);
				
			}
		};
	
	}
}