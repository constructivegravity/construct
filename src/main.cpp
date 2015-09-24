#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/permutation.hpp>

#include <iostream>

int main(int argc, char** argv) {
	
	auto indices = Albus::Tensor::Indices::GetRomanSeries(3, {1,3});
	
	std::cout << indices << std::endl;
	std::cout << Albus::Tensor::Permutation::Cyclic(indices) << std::endl;
	
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