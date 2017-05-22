#define RECOVER_FROM_EXCEPTIONS 	0

#include <common/logger.hpp>

#include <equations/equations.hpp>
#include <tensor/expression_database.hpp>
#include <common/progressbar.hpp>

int main(int argc, char** argv) {
    std::cerr << "The infamous Apple Program" << std::endl;
    std::cerr << "(c) 2016 Constructive Gravity Group Erlangen" << std::endl;
    std::cerr << "All rights reserved." << std::endl << std::endl;

    std::cerr << "        ,---,_          ,          " << std::endl;
    std::cerr << "         _>   `'-.  .--'/          " << std::endl;
    std::cerr << "    .--'` ._      `/   <_          " << std::endl;
    std::cerr << "     >,-' ._'.. ..__ . ' '-.       " << std::endl;
    std::cerr << "  .-'   .'`         `'.     '.     " << std::endl;
    std::cerr << "   >   / >`-.     .-'< \\ , '._\\  " << std::endl;
    std::cerr << "  /    ; '-._>   <_.-' ;  '._>     " << std::endl;
    std::cerr << "  `>  ,/  /___\\ /___\\  \\_  /    " << std::endl;
    std::cerr << "  `.-|(|  \\o_/  \\o_/   |)|`      " << std::endl;
    std::cerr << "      \\;        \\      ;/        " << std::endl;
    std::cerr << "        \\  .-,   )-.  /           " << std::endl;
    std::cerr << "         /`  .'-'.  `\\            " << std::endl;
    std::cerr << "        ;_.-`.___.'-.;             " << std::endl << std::endl;

    Construction::Logger::Screen("screen");
    Construction::Logger::File("file", "apple.log");

    Construction::Logger logger;
    logger.SetDebugLevel("screen", Construction::Common::DebugLevel::WARNING);

    if (argc < 2) {
        logger << Construction::Logger::ERROR << "You need to specify the file that needs to be solved" << Construction::Logger::endl;
        return -1;
    }

    // Add options for debugging
    Construction::Equations::SubstitutionManager::Instance()->SetMaxTickets(1);

    if (argc > 2) {
        std::string option = argv[2];

        if (option == "-d" || option == "--debug") {
            logger.SetDebugLevel("screen", Construction::Common::DebugLevel::DEBUG);
        }

        // Set the number of cores
        int maxTickets = std::atoi(argv[2]);
        Construction::Equations::SubstitutionManager::Instance()->SetMaxTickets(maxTickets);
    }

    if (argc > 3) {
        std::string option = argv[3];

        if (option == "-d" || option == "--debug") {
            logger.SetDebugLevel("screen", Construction::Common::DebugLevel::DEBUG);
        }
    }

    logger << Construction::Logger::DEBUG << "Start to solve file `" << argv[1] << "`" << Construction::Logger::endl;

    Construction::Common::TimeMeasurement time;

    // Initialize database
    Construction::Tensor::ExpressionDatabase::Instance()->Initialize("construct.db");
    Construction::Tensor::ExpressionDatabase::Instance()->Deactivate(); // Deactivate for now

    // Open file
    std::ifstream file (argv[1]);
    if (!file.is_open()) return -1;

    std::vector<std::shared_ptr<Construction::Equations::Equation>> equations;

    // Read line per line and add them as equations
    std::string line;
    while (std::getline(file, line)) {
        // Ignore empty lines
        if (line == "") continue;

        // Add the equation
        auto eq = std::make_shared<Construction::Equations::Equation>(line);

        // If the equation isn't empty, add it
        if (!eq->IsEmpty()) {
            equations.push_back(std::move(eq));
        }
    }

    // Print the code
    for (auto& eq : equations) {
        std::cerr<< " \033[36m" << "> " << eq->ToLaTeX() << "\033[0m" << std::endl;
    }

    // Calculate number of coefficients and equations
    int numberOfSteps = equations.size() + 8 * Construction::Equations::Coefficients::Instance()->Size();

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
    logger << Construction::Logger::INFO << "Start calculating ..." << Construction::Logger::endl;

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

    // Collect all the variables in the coefficients
    std::vector<Construction::Tensor::Scalar> variables;
    for (auto it = Construction::Equations::Coefficients::Instance()->begin(); it != Construction::Equations::Coefficients::Instance()->end(); ++it) {
        auto tensor = *it->second->Get();
        auto stuff = tensor.ExtractVariables();
        for (auto& pair : stuff) {
            auto _it = std::find(variables.begin(), variables.end(), pair.first);
            if (_it == variables.end()) variables.push_back(pair.first);
        }
    }

    // Build a substitution
    Construction::Tensor::Substitution substitution;
    int pos = 1;
    for (auto& variable : variables) {
        substitution.Insert(variable, Construction::Tensor::Scalar::Variable("e", pos++));
    }

    // Print the results
    for (auto it = Construction::Equations::Coefficients::Instance()->begin(); it != Construction::Equations::Coefficients::Instance()->end(); ++it) {
        //auto tensor = it->second->Get()->RedefineVariables("e", offset);
        //auto tensor = substitution(*it->second->GetAsync()).CollectByVariables();
        auto tensor = substitution(*it->second->GetAsync()).Simplify();

        auto summands = tensor.GetSummands();

        if (!tensor.IsZeroTensor()) {
            offset += summands.size();
        }

        std::cout << "  \033[36m#<" << it->first.id << ":" << it->first.l << ":" << it->first.ld << ":" << it->first.r << ":" << it->first.rd << ">" << "\033[0m = " << std::endl;

        for (int i=0; i<summands.size(); ++i) {
            auto t = summands[i];

            if (t.IsScaled()) {
                auto s = t.SeparateScalefactor();
                std::cout << "     \033[32m";

                if (s.first.IsAdded()) {
                    std::cout << "(" << s.first << ")";
                } else {
                    std::cout << s.first;
                }

                std::cout << "\033[0m * \033[33m";

                if (s.second.IsAdded()) {
                    std::cout << "(" << s.second << ")";
                } else std::cout << s.second;
            } else if (t.IsScalar()) {
                std::cout << "     \033[32m" << t.ToString();
            } else {
                std::cout << "     \033[33m" << t.ToString();
            }

            std::cout << "\033[0m";

            if (i < summands.size()-1) std::cout << " + ";

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }

    // Finally check all the coefficients
    /*
    {
        Construction::Logger::Debug("Run a final test ...");
        int violations = 0;
        for (auto &eq : equations) {
            Construction::Tensor::Tensor result;

            // Check the equation
            auto correct = eq->Test(&result);

            if (!correct) {
                Construction::Logger::Error("Equation `", eq->ToLaTeX(), "` is violated.\nFound ", result);
                ++violations;
            }
        }
        Construction::Logger::Debug("Found ", violations, " violation(s)");
    }
     */

    time.Stop();
    std::cerr << time << std::endl;

    std::cerr << "Finished." << std::endl;
}
