#pragma once

#include <common/error.hpp>

#include <language/parser.hpp>
#include <language/session.hpp>
#include <language/notebook.hpp>

#include <language/argument.hpp>
#include <language/command.hpp>

#include <language/tensor.hpp>
#include <language/symmetrization.hpp>
#include <language/linear_dependent.hpp>

#include <tensor/expression.hpp>
#include <generator/basis_selector.hpp>
#include <generator/linear_dependent_selector.hpp>
#include <generator/symmetrized_tensor.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace Construction {
    namespace Language {

        class WrongTypeException : public Exception {
        public:
            WrongTypeException() : Exception("Unexpected type error") { }
        };

        class CLI {
        public:
            CLI(int argc, char** argv) {
                auto crashFile = boost::filesystem::system_complete(argv[0]).remove_filename().string();
                if (crashFile[crashFile.size()-1] != '/') crashFile += "/";
                crashFile += ".crashfile";
            }

            ~CLI() {
                //database.SaveToFile("tensors.db");
            }
        public:
            void Error(const std::string& message) const {
                std::cout << "\033[31m" << "Error: " << "\033[0m" << message << std::endl;
            }
        public:
            std::string GetExpandedCommandString(const std::shared_ptr<Node>& document) const {
                if (document->IsPrevious()) {
                    return lastCmd;
                } else if (document->IsCommand()) {
                    auto commandName = std::dynamic_pointer_cast<CommandNode>(document)->GetIdentifier()->GetText();

                    CommandPointer command;

                    std::string result = commandName;
                    result.append("(");

                    auto args = std::dynamic_pointer_cast<CommandNode>(document)->GetArguments();

                    bool first = true;
                    for (auto& arg : *args) {
                        // Add comma to the previous argument
                        if (!first) result.append(",");
                        else first = false;

                        // If the argument is another command, call this first
                        if (arg->IsCommand()) {
                            // Update newLastCmd string
                            // Note that Execute has overwritten lastCmd
                            result.append(GetExpandedCommandString(arg));
                        }
                        // If pointer to previous, add this
                        else if (arg->IsPrevious()) {
                            // Update newLastCmd string
                            result.append(lastCmd);
                        }
                        // If is a literal, load the name from memory
                        else if (arg->IsLiteral()) {
                            auto id = arg->ToString();
                            auto it = definition.find(id);
                            if (it == definition.end()) throw Exception("The symbol is not known");
                            result.append(it->second);
                        }
                        // If indices, add this
                        else if (arg->IsIndices()) {
                            // Update command
                            result.append("\"");
                            result.append(std::dynamic_pointer_cast<IndicesNode>(arg)->GetText());
                            result.append("\"");
                        }
                        // If string, add this
                        else if (arg->IsString()) {
                            // Update command
                            result.append("\'");
                            result.append(std::dynamic_pointer_cast<StringNode>(arg)->GetText());
                            result.append("\'");
                        }
                    }

                    // Append closing bracket
                    result.append(")");
                    return result;
                }
                // Literal
                else if (document->IsLiteral()) {
                    auto id = document->ToString();
                    auto it = definition.find(id);
                    if (it == definition.end()) return "";
                    return it->second;
                }
            }

            void Execute(const std::shared_ptr<Node>& document, bool silent=false) {
                // Copy the most recent expression;
                Expression lastResult = Session::Instance()->GetCurrent();
                Expression previousResult = lastResult;

                Common::TimeMeasurement time;

                if (document->IsPrevious()) {
                    PrintExpression(lastResult);
                    return;
                } else if (document->IsCommand()) {
                    auto commandName = std::dynamic_pointer_cast<CommandNode>(document)->GetIdentifier()->GetText();

                    std::string expandedCmd = GetExpandedCommandString(document);

                    // if the expandedCmd is in the database, do not evaluate
                    // 
                    // TODO: if the algorithms are out of the experimental stage, 
                    //       add the caching again
                    //
                    /*if (database.Contains(expandedCmd)) {
                        lastResult = database[expandedCmd];
                        lastCmd = expandedCmd;

                        if (!silent) PrintTensor();
                        return;
                    }*/

                    CommandPointer command;

                    try {
                        command = CommandManagement::Instance()->CreateCommand(commandName);
                    } catch (UnknownCommandException& err) {
                        Error("I do not know this command");
                        return;
                    }

                    // Apply arguments
                    auto args = std::dynamic_pointer_cast<CommandNode>(document)->GetArguments();

                    for (auto& arg : *args) {
                        // If the argument is another command, call this first
                        if (arg->IsCommand()) {
                            auto cmd = std::dynamic_pointer_cast<CommandNode>(arg);
                            Execute(cmd, true);

                            // Replace with a tensor argument reference to the previous result
                            lastResult = Session::Instance()->GetCurrent();

                            switch (lastResult.GetType()) {
                                // Tensor
                                {
                                    case Tensor::AbstractExpression::TENSOR:
                                        auto newArg = std::make_shared<TensorArgument>();
                                        newArg->SetTensor(lastResult.As<Tensor::Tensor>());
                                        command->AddArgument(std::move(newArg));
                                        break;
                                }

                                // Substitution
                                {
                                    case Tensor::AbstractExpression::SUBSTITUTION:
                                        auto newArg = std::make_shared<SubstitutionArgument>();
                                        newArg->SetSubstitution(lastResult.As<Tensor::Substitution>());
                                        command->AddArgument(std::move(newArg));
                                        break;
                                }

                                case Tensor::AbstractExpression::SCALAR:
                                    // TODO: implement scalar argument
                                    throw WrongTypeException();
                                    break;

                                default:
                                    throw WrongTypeException();
                                    break;
                            }
                        }
                        // If pointer to previous, add this
                        else if (arg->IsPrevious()) {
                            switch (previousResult.GetType()) {
                                {
                                    case Tensor::AbstractExpression::TENSOR:
                                        auto newArg = std::make_shared<TensorArgument>();
                                        newArg->SetTensor(previousResult.As<Tensor::Tensor>());
                                        command->AddArgument(std::move(newArg));
                                        break;
                                }

                                {
                                    case Tensor::AbstractExpression::SUBSTITUTION:
                                        auto newArg = std::make_shared<SubstitutionArgument>();
                                        newArg->SetSubstitution(previousResult.As<Tensor::Substitution>());
                                        command->AddArgument(std::move(newArg));
                                        break;
                                }

                                case Tensor::AbstractExpression::SCALAR:
                                    // TODO: implement scalar argument
                                    throw WrongTypeException();
                                    break;

                                default:
                                    throw WrongTypeException();
                                    break;
                            }
                        }
                        // If is a literal, load the name from memory
                        else if (arg->IsLiteral()) {
                            auto id = arg->ToString();
                            lastResult = Session::Instance()->Get(id);

                            switch (lastResult.GetType()) {
                                {
                                    case Tensor::AbstractExpression::TENSOR:
                                        auto newArg = std::make_shared<TensorArgument>();
                                        newArg->SetTensor(lastResult.As<Tensor::Tensor>());
                                        command->AddArgument(std::move(newArg));
                                        break;
                                }

                                {
                                    case Tensor::AbstractExpression::SUBSTITUTION:
                                        auto newArg = std::make_shared<SubstitutionArgument>();
                                        newArg->SetSubstitution(lastResult.As<Tensor::Substitution>());
                                        command->AddArgument(std::move(newArg));
                                        break;
                                }

                                case Tensor::AbstractExpression::SCALAR:
                                    // TODO: implement scalar argument
                                    throw WrongTypeException();
                                    break;

                                default:
                                    throw WrongTypeException();
                                    break;
                            }
                        }
                        // If indices, add this
                        else if (arg->IsIndices()) {
                            command->AddArgument(std::make_shared<IndexArgument>(
                                    std::dynamic_pointer_cast<IndicesNode>(arg)->GetText()
                            ));
                        }
                        // If string, add this
                        else if (arg->IsString()) {
                            command->AddArgument(std::make_shared<StringArgument>(
                                    std::dynamic_pointer_cast<StringNode>(arg)->GetText()
                            ));
                        } else if (arg->IsNumeric()) {
                            command->AddArgument(std::make_shared<NumericArgument>(
                                    std::dynamic_pointer_cast<NumericNode>(arg)->GetText()
                            ));
                        }
                    }

                    Expression newResult;

                    // Execute
                    try {
                        newResult = (*command)();

                        // Overwrite last result
                        if (!newResult.IsVoid()) {
                            lastResult = newResult;
                        }

                    } catch (WrongNumberOfArgumentsException err) {
                        Error("Wrong number of arguments");
                    } catch (WrongArgumentTypeException err) {
                        if (err.expected != "" && err.got != "") {
                            std::stringstream ss;
                            ss << "Wrong argument type (expected `" << err.expected << "`, got `" << err.got << "`)";
                            Error(ss.str());
                        } else {
                            Error("Wrong argument type");
                        }
                    }

                    // Stop time measurement
                    time.Stop();

                    // Update the session
                    if (!lastResult.IsVoid()) {
                        Session::Instance()->SetCurrent(expandedCmd, lastResult);
                    }

                    // Update database
                    /*if (command->Cachable()) {
                        database[lastCmd] = lastResult;
                    }*/

                    // Print tensors unless in silent mode
                    if (!silent) {
                        PrintExpression(newResult);

                        // Print the required time
                        std::cout << "\033[90m   " << time << "\033[0m" << std::endl;
                    }

                    return;
                } else if (document->IsAssignment()) {
                    auto id = std::dynamic_pointer_cast<AssignmentNode>(document)->GetIdentifier()->GetText();
                    auto expression = std::dynamic_pointer_cast<AssignmentNode>(document)->GetExpression();

                    Execute(expression, true);

                    // Update current
                    lastResult = Session::Instance()->GetCurrent();

                    // Store the variable in memory
                    Session::Instance()->Get(id) = lastResult;

                    definition[id] = lastCmd;
                    //database[lastCmd] = lastResult;

                    // Print tensors unless in silent mode
                    if (!silent) {
                        PrintExpression(lastResult);

                        time.Stop();

                        // Print the required time
                        std::cout << "\033[90m   " << time << "\033[0m" << std::endl;
                    }

                    return;
                } else if (document->IsLiteral()) {
                    auto id = std::dynamic_pointer_cast<LiteralNode>(document)->GetText();
                    Session::Instance()->SetCurrent(definition[id], Session::Instance()->Get(id));

                    // Update current
                    lastResult = Session::Instance()->GetCurrent();

                    if (!silent) PrintExpression(lastResult);
                    return;
                } else {
                    Error("Cannot execute this :'(");
                }
            }
        public:
            void operator()(const std::string& code) {
                std::string text = code;

                bool silent = false;

                if (code.size() > 0 && code[code.size() - 1] == ':') {
                    silent = true;
                    text = code.substr(0, code.size() - 1);
                }

                // Parse and get the AST
                auto document = parser.Parse(text);

                if (document == nullptr) {
                    Error("Something went wrong :/");
                    return;
                }

                #if RECOVER_FROM_EXCEPTIONS == 1
                try {
                #endif
                    Execute(document, silent);
                    Session::Instance()->GetNotebook().Append(code);
                #if RECOVER_FROM_EXCEPTIONS == 1    
                } catch (const std::bad_alloc& e) {
                    Error("Out of memory. :(");
                } catch (...) {
                    Error("Something went terribly wrong. :(");
                }
                #endif

                // Store the session on disk in order to recover in case of crash
                Session::Instance()->SaveToFile(crashFile);
            }

            void ExecuteScript(const std::string& filename, bool silent=false) {
                // Load text file
                std::ifstream file (filename);
                if (!file.is_open()) return;

                // Read line per line and execute
                std::string line;
                while (std::getline(file, line)) {
                    // Ignore empty lines
                    if (line == "") continue;

                    // Ignore comments
                    if (line[0] == '#') continue;

                    // Turn silent?
                    if (silent && line[line.size()-1] != ':') line.append(":");

                    // Print line to see
                    std::cout << "> " << line << std::endl;

                    try {
                        // Execute
                        (*this)(line);
                    } catch (...) {
                        // If an exception is thrown, exit since further evaluation
                        // of the scripts cannot be garantueed.
                        Error("Cannot recover from this. Stopping execution of the script ...");
                        return;
                    }
                }
            }
        public:
            void PrintExpression(const Expression& expression) {
                /**
                    DEFAULT = 39,
                    BLACK = 30,
                    RED = 31,
                    GREEN = 32,
                    YELLOW = 33,
                    BLUE = 34,
                    MAGENTA = 35,
                    CYAN = 36,
                    LIGHTGRAY = 37,
                    DARKGRAY = 90,
                    LIGHTRED = 91,
                    LIGHTGREEN = 92,
                    LIGHTYELLOW = 93,
                    LIGHTBLUE = 94,
                    LIGHTMAGENTA = 95,
                    LIGHTCYAN = 96,
                    WHITE = 97
                 */
                //std::stringstream ss;
                //lastResult.Serialize(ss);

                // Get the output
                std::stringstream ss;
                ss << expression.ToString();
                std::string line;

                // Change the output color
                std::cout << "\033[" << expression.GetColorCode() << "m";

                // Shift the output by three characters
                while (std::getline(ss, line)) {
                    std::cout << "   " << line << std::endl;
                }
                
                // Change the color back
                std::cout << "\033[0m";
            }
        private:
            Parser parser;

            //TensorDatabase database;

            // Store the command to that was executed to create %
            std::string lastCmd;
            std::string crashFile;
            std::map<std::string, std::string> definition;
        };

    }
}
