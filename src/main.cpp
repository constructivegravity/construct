#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/permutation.hpp>

#include <iostream>
#include <iomanip>
#include <cassert>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
	os << "[";
	for (int i=0; i<vec.size(); i++) {
		os << vec[i];
		if (i != vec.size()-1) os << ", ";
	}
	os << "]";
	return os;
}

int main(int argc, char** argv) {

	//auto epsilon = Albus::Tensor::EpsilonTensor::InSpaceTime();
	//std::cout << epsilon(0,0,1,2) << std::endl;

	/*std::vector<int> vec  = { 0, 1, 2, 3};
	std::vector<int> vec2 = { 1, 2, 3, 0};

	auto permutation = Albus::Tensor::Permutation::From(vec2, vec);

	std::cout << vec2 << " => " << vec << std::endl << std::endl;
	std::cout << permutation << std::endl;

	std::cout << permutation(vec2) << std::endl;
	std::cout << permutation.Sign() << std::endl;

	 */
	/*

	auto epsilon = Albus::Tensor::EpsilonTensor::InSpaceTime();
	std::cout << epsilon << std::endl;

	/**
	auto indices = Albus::Tensor::Indices::GetRomanSeries(4, {0,3});
	
	std::cout << indices << std::endl;
	std::cout << Albus::Tensor::Permutation::Cyclic(indices) << std::endl;

	std::cout << "Is permutation: " << indices.IsPermutationOf(Albus::Tensor::Indices::GetGreekSeries(4, {0,3})) << std::endl;
	
	auto vec = Albus::Tensor::Permutation::Cyclic(1,3,2);
	for (auto& i : vec) {
		std::cout << i << " ";
	}
	std::cout << std::endl;
	
	/**
	    ( a , b , c , d )
	         ===> ?
		( d , a , b , c)
	
		(1,2) , (1,3) , (1,4)
	
	    Check:
			( a , b , c , d)
	    ==> ( b , a , c , d)
	    ==> ( c , a , b , d)
	    ==> ( d , a , b , c)
	 */
	
	/**auto indices = Albus::Tensor::Indices({
		{ "mu", "\\mu", {1,3} },
		{ "nu", "\\nu", {1,3} }
	});
	
	auto tensor = Albus::Tensor::Tensor("epsilon", "\\epsilon", indices);
	std::cout << tensor << std::endl;
	
	tensor(1,1);
	*/
	//indices(0,0);
}