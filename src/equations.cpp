#define RECOVER_FROM_EXCEPTIONS 	1

#include <equations/equations.hpp>
#include <common/progressbar.hpp>

int main(int argc, char** argv) {
    std::cerr << "The infamous Apple Program" << std::endl;
    std::cerr << "(c) 2016 Constructive Gravity Group Erlangen" << std::endl;
    std::cerr << "All rights reserved." << std::endl << std::endl;

    if (argc < 2) {
        std::cerr << "You need to specify the file that needs to be solved" << std::endl;
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

    // Calculate number of coefficients and equations
    int numberOfSteps = equations.size() + Construction::Equations::Coefficients::Instance()->Size();

    // Create progress bar
    Construction::Common::ProgressBar progress (numberOfSteps, 100);

    // Register an observer for the coefficients
    for (auto it = Construction::Equations::Coefficients::Instance()->begin(); it != Construction::Equations::Coefficients::Instance()->end(); ++it) {
        it->second->RegisterObserver([&](const Construction::Equations::CoefficientReference&) {
            progress++;
        });
    }

    // Register an observer for the equations
    for (auto& eq : equations) {
        eq->RegisterObserver([&](const Construction::Equations::Equation&) {
            progress++;
        });
    }

    // Start progress bar
    std::cerr << "Start calculating ..." << std::endl;
    progress.Start();

    // Start the generation of all required coefficients
    Construction::Equations::Coefficients::Instance()->StartAll();

    // Wait for all equations to be solved
    for (auto& eq : equations) {
        eq->Wait();
    }

    // Print the result
    int offset = 0;

    // Clean the line
    for (int i=0; i<200; i++) {
        std::cerr << " ";
    }
    std::cerr << "\r";

    // Print the results
    for (auto it = Construction::Equations::Coefficients::Instance()->begin(); it != Construction::Equations::Coefficients::Instance()->end(); ++it) {
        //auto tensor = it->second->Get()->RedefineVariables("e", offset);
        auto tensor = *it->second->Get();

        if (!tensor.IsZeroTensor()) {
            offset += tensor.GetSummands().size();
        }

        std::cout << "  #<" << it->first.id << ":" << it->first.l << ":" << it->first.ld << ":" << it->first.r << ":" << it->first.rd << ">" << " = " << tensor << std::endl << std::endl;
    }

    std::cerr << "Finished." << std::endl;
}
