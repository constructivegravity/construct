#include <equations/equations.hpp>

int main(int argc, char** argv) {
    std::cout << "The infamous Apple Program" << std::endl;
    std::cout << "(c) 2016 Constructive Gravity Group Erlangen" << std::endl;
    std::cout << "All rights reserved." << std::endl << std::endl;

    if (argc < 2) {
        std::cout << "You need to specify the file that needs to be solved" << std::endl;
        return -1;
    }

    // Open file
    std::ifstream file (argv[1]);
    if (!file.is_open()) return -1;

    std::vector<std::unique_ptr<Construction::Equations::Equation>> equations;

    // Read line per line and add them as equations
    std::string line;
    while (std::getline(file, line)) {
        equations.push_back(
            std::unique_ptr<Construction::Equations::Equation>(new Construction::Equations::Equation(line))
        );
    }

    // Start the generation of all required coefficients
    Construction::Equations::Coefficients::Instance()->StartAll();

    // Wait for all equations to be solved
    for (auto& eq : equations) {
        eq->Wait();
    }

    // Print the result
    // TODO: print the result

    std::cout << "Finished." << std::endl;
}
