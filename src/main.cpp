#include <common/time_measurement.hpp>

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/permutation.hpp>

#include <vector/matrix.hpp>

#include <generator/base_tensor.hpp>

#include <iostream>
#include <iomanip>
#include <cassert>
#include <fstream>
#include <iostream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

/*
template<typename T, typename S>
std::ostream& operator<<(std::ostream& os, const std::pair<S,T>& vec) {
	os << "(" << vec.first << ", " << vec.second << ")";
	return os;
}

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
*/

std::ostream& operator<<(std::ostream& os, const std::vector<TensorPointer>& vec) {
	os << "[";
	for (int i=0; i<vec.size(); i++) {
		os << vec[i]->ToString();
		if (i != vec.size()-1) os << ", ";
	}
	os << "]";
	return os;
}

int main(int argc, char** argv) {

	Albus::Vector::Matrix M = {
			{ 1, 3, 5 },
			{ 5, -2, 7},
			{ 6, 1, 12},
			{ 7, 1, 8},
	};

	std::cout << M << std::endl;
	std::cout << M.GetRowEchelonForm() << std::endl;

	/*if (argc < 2) {
		std::cout << "Usage: albus <order>" << std::endl;
		return -1;
	}

	int order = atoi(argv[0]);

	Albus::Common::TimeMeasurement time;

	Albus::Generator::BaseTensorGenerator generator;
	auto tensors = generator.Generate(order);

	time.Stop();

	std::cout << std::setw(5) << order << std::setw(10) << tensors.size() << std::setw(20) << time << std::endl;*/

}