#pragma once

#include <cobalt.hpp>

#define RECOVER_FROM_EXCEPTIONS 	0
#define DEBUG_MODE

#include <regex>
#include <common/logger.hpp>

#include <equations/equations.hpp>
#include <tensor/expression_database.hpp>
#include <common/progressbar.hpp>

namespace Construction {
    namespace Cmd {

        std::string TrimLeft(const std::string& str, char c = ' ') {
            std::string output = str;

            while (output.size() > 0 && output[0] == c) {
                output = output.substr(1);
            }

            return output;
        }

        std::string TrimRight(const std::string& str, char c = ' ') {
            std::string output = str;

            while (output.size() > 0 && output[output.size()-1] == c) {
                output = output.substr(0, output.size()-1);
            }

            return output;
        }

        std::string Trim(const std::string& str, char c = ' ') {
            return TrimLeft(TrimRight(str, c), c);
        }

        class SolveCommand : public Cobalt::Command<SolveCommand> {
        public:
            static std::string Use() {
                return "solve [filename]";
            }

            static std::string Short() {
                return "Solve a script file and print the result";
            }

            static std::string Long() {
                return "Solve a script file given as argument and print the final result.";
            }

            void RegisterFlags() {
                AddLocalFlag<int>(parallelEqns, "parallel", "p", 1, "Number of equations that are solved in parallel");
                AddLocalFlag<bool>(abc, "abc", "a", false, "Do not print the full tensors but only the scalars in front of base tensors");
                AddLocalFlag<bool>(colored, "colored", "c", false, "Prettify the output");
            }

            int Run(const Cobalt::Arguments& args) {
                Construction::Logger::Screen("screen");
                Construction::Logger::File("file", "apple.log");

                Construction::Logger logger;
                logger.SetDebugLevel("screen", Construction::Common::DebugLevel::WARNING);

                if (args.size() == 0) {
                    Construction::Logger::Error("You need to specify a file to solve");
                    return -1;
                }

                // Add options for debugging
                Construction::Equations::SubstitutionManager::Instance()->SetMaxTickets(parallelEqns);

                if (Lookup<bool>("debug")) {
                    logger.SetDebugLevel("screen", Construction::Common::DebugLevel::DEBUG);
                }

                logger << Construction::Logger::DEBUG << "Start to solve file `" << args[0] << "`" << Construction::Logger::endl;

                Construction::Common::TimeMeasurement time;

                // Initialize database
                Construction::Tensor::ExpressionDatabase::Instance()->Initialize("construct.db");
                Construction::Tensor::ExpressionDatabase::Instance()->Deactivate(); // Deactivate for now

                // Open file
                std::ifstream file (args[0]);
                if (!file.is_open()) return -1;

                std::vector<std::shared_ptr<Construction::Equations::Equation>> equations;

                // Read line per line and add them as equations
                std::string line;
                while (std::getline(file, line)) {
                    // Trim lines to deal with Refik's input
                    line = Trim(line);

                    // Delete all "\r"s
                    line = std::regex_replace(line, std::regex("\\r"), "");

                    // Ignore empty lines
                    if (line == "") continue;

                    // Ignore comments
                    if (line.size() > 1 && line[0] == '/' && line[1] == '/') continue;

                    // Add the equation
                    auto eq = std::make_shared<Construction::Equations::Equation>(line);

                    // If the equation isn't empty, add it
                    if (!eq->IsEmpty()) {
                        equations.push_back(std::move(eq));
                    }
                }

                // Print all the coefficients
                std::cerr << (colored  ? "\033[32m" : "") << "Coefficients:" << (colored? "\033[0m" : "") << std::endl;
                for (auto it = Construction::Equations::Coefficients::Instance()->begin(); it != Construction::Equations::Coefficients::Instance()->end(); ++it) {
                    std::cout << (colored ? "  \033[36m" : "") << it->second->ToString(false) << (colored ? "\033[0m" : "") << std::endl;
                }
                std::cerr << std::endl;

                // Print the code
                for (auto& eq : equations) {
                    std::cerr<< (colored ? " \033[36m" : " ") << "> " << eq->ToLaTeX() << "\033[0m" << std::endl;
                }
                std::cerr << std::endl;

                // Calculate number of coefficients and equations
                int numberOfSteps = equations.size() + Construction::Equations::Coefficients::Instance()->GetNumberOfSteps();

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
                if (!abc) {
                    for (auto it = Construction::Equations::Coefficients::Instance()->begin(); it != Construction::Equations::Coefficients::Instance()->end(); ++it) {
                    //auto tensor = it->second->Get()->RedefineVariables("e", offset);
                    //auto tensor = substitution(*it->second->GetAsync()).CollectByVariables();
                    auto tensor = substitution(*it->second->GetAsync());//.Simplify();

                    auto summands = tensor.GetSummands();

                    if (!tensor.IsZeroTensor()) {
                        offset += summands.size();
                    }

                    std::cout << (colored ? "  \033[36m" : "") << it->second->ToString(false) << (colored ? "\033[0m" : "") << " = " << std::endl;

                    for (int i=0; i<summands.size(); ++i) {
                        auto t = summands[i];

                        if (t.IsScaled()) {
                            auto s = t.SeparateScalefactor();
                            s.first = s.first.Simplify();

                            std::cout << (colored ? "     \033[32m" : "     ");

                            if (s.first.IsAdded()) {
                                std::cout << "(" << s.first << ")";
                            } else {
                                std::cout << s.first;
                            }

                            std::cout << (colored ? "\033[0m" : "") << " * " << (colored ? "\033[33m" : "");

                            if (s.second.IsAdded()) {
                                std::cout << "(" << s.second << ")";
                            } else std::cout << s.second;
                        } else if (t.IsScalar()) {
                            std::cout << "     " << (colored ? "\033[32m" : "") << t.ToString();
                        } else {
                            std::cout << "     " << (colored ? "\033[33m" : "") << t.ToString();
                        }

                        if (colored) std::cout << "\033[0m";

                        if (i < summands.size()-1) std::cout << " + ";

                        std::cout << std::endl;
                    }

                    std::cout << std::endl;
                }
                } else {
    int coeffPos = 1;
    for (auto it = Construction::Equations::Coefficients::Instance()->begin(); it != Construction::Equations::Coefficients::Instance()->end(); ++it) {
        //auto tensor = it->second->Get()->RedefineVariables("e", offset);
        //auto tensor = substitution(*it->second->GetAsync()).CollectByVariables();
        auto tensor = substitution(*it->second->GetAsync()).Simplify();

        auto summands = tensor.GetSummands();

        if (!tensor.IsZeroTensor()) {
            offset += summands.size();
        }

        std::cout << (colored ? "  \033[36m" : "") << it->second->ToString(false) << (colored ? "\033[0m" : "") << " : " << std::endl;

        for (int i=0; i<summands.size(); ++i) {
            auto t = summands[i];

            if (t.IsScaled()) {
                auto s = t.SeparateScalefactor();
                auto abc = Construction::Tensor::Scalar(s.second.GetSummands().size(), 1) * s.first;

                std::cout << "     " << (colored ? "\033[32m" : "");

                char c = 'a' + static_cast<char>(i);

                std::cout << c << coeffPos << " = " << abc.ToString();
            } else if (t.IsScalar()) {
                std::cout << "     " << (colored ? "\033[32m" : "") << t.ToString();
            } else {
                std::cout << "     " << (colored ? "\033[33m" : "") << t.ToString();
            }

            if (colored) std::cout << "\033[0m";

            std::cout << std::endl;
        }

        std::cout << std::endl;
        coeffPos++;
                }

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

                return 0;
            }
        private:
            int parallelEqns;
            bool abc;
            bool colored;
        };

    }
}
