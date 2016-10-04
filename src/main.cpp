#define RECOVER_FROM_EXCEPTIONS 	0


#include <common/time_measurement.hpp>

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/permutation.hpp>
#include <tensor/symmetrization.hpp>

#include <tensor/index_parser.hpp>

#include <language/cli.hpp>
#include <language/command.hpp>

#include <vector/matrix.hpp>
//#include <vector/linear_system.hpp>

#include <generator/base_tensor.hpp>

#include <iostream>
#include <iomanip>
#include <cassert>
#include <fstream>
#include <string>
#include <cstdio>

#include <readline/readline.h>
#include <readline/history.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

using namespace Construction::Tensor;

/*BOOST_CLASS_EXPORT_GUID(Construction::Tensor::AddedTensor, "Construction::Tensor::AddedTensor")
BOOST_CLASS_EXPORT_GUID(Construction::Tensor::MultipliedTensor, "Construction::Tensor::MultipliedTensor")
BOOST_CLASS_EXPORT_GUID(Construction::Tensor::ScaledTensor, "Construction::Tensor::ScaledTensor")
BOOST_CLASS_EXPORT_GUID(Construction::Tensor::ScalarTensor, "Construction::Tensor::ScalarTensor")
BOOST_CLASS_EXPORT_GUID(Construction::Tensor::GammaTensor, "Construction::Tensor::GammaTensor")*/
//BOOST_CLASS_EXPORT_GUID(Construction::Tensor::EpsilonGammaTensor, "Construction::Tensor::EpsilonGammaTensor")


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
		//os << vec[i]->ToString();
		if (i != vec.size()-1) os << ", ";
	}
	os << "]";
	return os;
}

char* command_iterator(const char* text, int state) {
	static std::vector<std::string>::iterator it;
	auto cmds = Construction::Language::CommandManagement::Instance()->GetCommandList();

	if (state == 0) it = begin(cmds);

	while (it != end(cmds)) {
		auto& cmd = *it;
		++it;

		if (cmd.find(text) != std::string::npos) {
			char* completion = new char[cmd.size()];
			strcpy(completion, cmd.c_str());
			return completion;
		}
	}
	return nullptr;
}

char** getCommandCompletions(const char* text, int start, int) {
	char** completionList = nullptr;

	if (start == 0)
		completionList = rl_completion_matches(text, &command_iterator);

	return completionList;
}

int main(int argc, char** argv) {

	Construction::Language::CLI cli (argc, argv);

	// Hook readline completion
	rl_attempted_completion_function = &getCommandCompletions;

	// If there is a filename given, execute this
	if (argc >= 2) {
		std::string filename = argv[1];
		cli.ExecuteScript(filename);
		return 0;
	}

	// Create path for crashfile
	std::string crashFile = ".crashfile";
	/*auto crashFile = boost::filesystem::system_complete(argv[0]).remove_filename().string();
    if (crashFile[crashFile.size()-1] != '/') crashFile += "/";
    crashFile += ".crashfile";*/

	// If the file exists
	if (boost::filesystem::exists( crashFile )) {
		// Ask if the session should be restored
		std::cout << "Construction can restore the previous session. Should it? [Y/n]: ";

		while (true) {
			std::string input;
			std::cin >> input;

			if (input == "Y") {
				Construction::Language::Session::Instance()->LoadFromFile(crashFile);
				break;
			} else if (input == "n") break;
			else
				std::cout << "Construction can restore the previous session. Should it? [Y/n]: ";
		}
	}

	// CLI mode

	std::string input;
	while (true) {
		input = std::string(readline("> "));

		if (input == "Exit") {
			// Delete crash file
			remove(crashFile.c_str());

			std::cout << "Bye!" << std::endl;
			break;
		}

		if (input != "") {
			add_history(input.c_str());
			cli(input);
		}
	}

	/*if (argc < 2) {
		std::cout << "Usage: Construction <order>" << std::endl;
		return -1;
	}*/

	/*Indices indices;
	indices.Insert(Index("c", {1,3}));
	indices.Insert(Index("b", {1,3}));
	indices.Insert(Index("a", {1,3}));
	indices.Insert(Index("d", {1,3}));
	std::cout << indices << std::endl;
	std::cout << indices.Ordered() << std::endl;
	auto permutation = Permutation::From(indices, Indices::GetRomanSeries(4, {1,3}));
	std::cout << permutation << std::endl;
	std::cout << indices << " <-> " << permutation(indices) << std::endl;
	std::cout << permutation.Sign() << std::endl;*/

	/*Construction::Tensor::IndexParser parser;
	parser.Parse("(a b) c (d e) f");*/

	// (a|:1 (b|:2 |c):1 |d):2 e => abcde with symmetries (ac) and (bd)

	/*int order = (argc >= 2) ? atoi(argv[1]) : 7;
	Construction::Common::TimeMeasurement time;
	Construction::Generator::BaseTensorGenerator generator;
	Construction::Generator::BasisSelector selector;
	auto tensors = generator.Generate(order);
	time.Stop();
	std::cerr << std::setw(5) << order << std::setw(10) << tensors.Size() << std::setw(20) << time << std::endl;
	TensorContainer symmetrized;
	std::cerr << tensors[0].GetIndices() << std::endl;
	// Symmetrize
	{
		Indices toSymmetrize = Construction::Tensor::Indices::GetRomanSeries(2, {1,3});
		std::cout << toSymmetrize << std::endl;
		Construction::Generator::SymmetrizedTensorGenerator symmetrizer(toSymmetrize);
		Construction::Common::TimeMeasurement time2;
		symmetrized = symmetrizer(tensors);
		time2.Stop();
		std::cerr << std::setw(5) << order << std::setw(10) << symmetrized.Size() << std::setw(20) << time2 <<
		std::endl;
		for (auto& tensor : symmetrized) {
			std::cerr << "  " << tensor->ToString() << std::endl;
		}
	}
	std::cerr << std::endl;
	// Symmetrize second
	{
		Indices toSymmetrize = Construction::Tensor::Indices::GetRomanSeries(2, {1,3}, 3);
		std::cerr << toSymmetrize << std::endl;
		Construction::Generator::SymmetrizedTensorGenerator symmetrizer(toSymmetrize);
		Construction::Common::TimeMeasurement time2;
		symmetrized = symmetrizer(symmetrized);
		time2.Stop();
		std::cerr << std::setw(5) << order << std::setw(10) << symmetrized.Size() << std::setw(20) << time2 <<
		std::endl;
		//std::cout << "\\begin{eqnarray}" << std::endl;
		for (auto& tensor : symmetrized) {
			//std::cout << tensor->ToString() << std::endl;
			std::cout << tensor->ToString() << " \\\\" << std::endl;
		}
		//std::cout << "\\end{eqnarray}" << std::endl;
	}
	// Symmetrize third
	{
		Indices toSymmetrize = Construction::Tensor::Indices::GetRomanSeries(2, {1,3}, 5);
		std::cerr << toSymmetrize << std::endl;
		Construction::Generator::SymmetrizedTensorGenerator symmetrizer(toSymmetrize);
		Construction::Common::TimeMeasurement time2;
		symmetrized = symmetrizer(symmetrized);
		time2.Stop();
		std::cerr << std::setw(5) << order << std::setw(10) << symmetrized.Size() << std::setw(20) << time2 <<
		std::endl;
		//std::cout << "\\begin{eqnarray}" << std::endl;
		for (int i=0; i<symmetrized.Size(); i++) {
			//std::cout << tensor->ToString() << std::endl;
			std::cout << "\033[34m" << "c_" << (i+1) << "\\cdot\\left(" << symmetrized[i].ToString() << "\\right) \\\\ \033[0m" << std::endl;
		}
		//std::cout << "\\end{eqnarray}" << std::endl;
	}*/
/*
	// Symmetrize last
	{
		Indices toSymmetrize = Construction::Tensor::Indices::GetRomanSeries(2, {1,3}, 6);
		std::cerr << toSymmetrize << std::endl;
		Construction::Generator::SymmetrizedTensorGenerator symmetrizer(toSymmetrize);
		Construction::Common::TimeMeasurement time2;
		symmetrized = symmetrizer(symmetrized);
		time2.Stop();
		std::cerr << std::setw(5) << order << std::setw(10) << symmetrized.Size() << std::setw(20) << time2 <<
		std::endl;
		//std::cout << "\\begin{eqnarray}" << std::endl;
		for (auto& tensor : symmetrized) {
			std::cout << tensor->ToString() << std::endl;
			//std::cout << tensor->ToString() << " \\\\" << std::endl;
		}
		//std::cout << "\\end{eqnarray}" << std::endl;
	}*/

	/*Construction::Common::TimeMeasurement time2;
	auto basis = selector(tensors);
	time2.Stop();
	std::cerr << std::setw(5) << order << std::setw(10) << basis.Size() << std::setw(20) << time2 << std::endl << std::endl;
	std::cout << "\\begin{eqnarray}" << std::endl;
	for (auto& tensor : tensors) {
		std::cout << "  " << tensor->ToString() << " \\\\"<< std::endl;
	}
	std::cout << "\\end{eqnarray}" << std::endl;*/
}
