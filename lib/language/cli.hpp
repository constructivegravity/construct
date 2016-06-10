#pragma once

#include <language/parser.hpp>
#include <language/session.hpp>
#include <language/notebook.hpp>

#include <language/argument.hpp>
#include <language/command.hpp>

#include <language/tensor.hpp>
#include <language/symmetrization.hpp>
#include <language/linear_dependent.hpp>

#include <tensor/tensor_container.hpp>
#include <tensor/tensor_database.hpp>
#include <generator/basis_selector.hpp>
#include <generator/linear_dependent_selector.hpp>
#include <generator/symmetrized_tensor.hpp>

using Construction::Tensor::TensorDatabase;
using Construction::Tensor::TensorContainer;

namespace Construction {
    namespace Language {

        class CLI {
        public:
            CLI(bool forceNewDatabase=false) {
                if (!forceNewDatabase) {
                    // Check if file exists
                    std::ifstream file("tensors.db");
                    if (file) {
                        file.close();
                        try {
                            //database.LoadFromFile("tensors.db");
                        } catch (...) {
                            database.Clear();
                        }
                    } else file.close();
                }
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
                // Copy the most recent tensor;
                TensorContainer lastResult = Session::Instance()->GetCurrent();

                if (document->IsPrevious()) {
                    PrintTensor();
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
                            auto newArg = std::make_shared<TensorArgument>();
                            newArg->SetTensor(lastResult);
                            command->AddArgument(std::move(newArg));
                        }
                        // If pointer to previous, add this
                        else if (arg->IsPrevious()) {
                            auto newArg = std::make_shared<TensorArgument>();
                            newArg->SetTensor(lastResult);
                            command->AddArgument(std::move(newArg));
                        }
                        // If is a literal, load the name from memory
                        else if (arg->IsLiteral()) {
                            auto id = arg->ToString();
                            lastResult = Session::Instance()->Get(id);

                            auto newArg = std::make_shared<TensorArgument>();
                            newArg->SetTensor(lastResult);
                            command->AddArgument(std::move(newArg));
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

                    // Execute
                    try {
                        // Only overwrite last result if the command returns ones
                        if (command->ReturnsTensors())
                            lastResult = (*command)();
                        else
                            (*command)();
                    } catch (WrongNumberOfArgumentsException err) {
                        Error("Wrong number of arguments");
                    } catch (WrongArgumentTypeException err) {
                        Error("Wrong argument type");
                    }

                    // Update the session
                    Session::Instance()->SetCurrent(expandedCmd, lastResult);

                    // Update database
                    /*if (command->Cachable()) {
                        database[lastCmd] = lastResult;
                    }*/

                    // Print tensors unless in silent mode
                    if (command->ReturnsTensors() && !silent) {
                        PrintTensor();
                    }

                    return;
                } else if (document->IsAssignment()) {
                    auto id = std::dynamic_pointer_cast<AssignmentNode>(document)->GetIdentifier()->GetText();
                    auto expression = std::dynamic_pointer_cast<AssignmentNode>(document)->GetExpression();

                    Execute(expression, true);

                    // Store the variable in memory
                    Session::Instance()->Get(id) = lastResult;

                    definition[id] = lastCmd;
                    //database[lastCmd] = lastResult;
                    return;
                } else if (document->IsLiteral()) {
                    auto id = std::dynamic_pointer_cast<LiteralNode>(document)->GetText();
                    Session::Instance()->SetCurrent(definition[id], Session::Instance()->Get(id));

                    if (!silent) PrintTensor();
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

                try {
                    Execute(document, silent);
                    Session::Instance()->GetNotebook().Append(code);
                } catch (...) {
                    Error("Something went terribly wrong. :(");
                }
            }

            void ExecuteScript(const std::string& filename, bool silent=false) {
                // Load text file
                std::ifstream file (filename);
                if (!file.is_open()) return;

                // Read line per line and execute
                std::string line;
                while (std::getline(file, line)) {
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
            void PrintTensor() {
                TensorContainer lastResult = Session::Instance()->GetCurrent();

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
                std::stringstream ss;
                lastResult.Serialize(ss);

                std::cout << "\033[32m";

                for (int i=0; i<lastResult.Size(); i++) {
                    if (lastResult[i].IsAddedTensor())
                        std::cout << "   " << "e_" << (i+1) << " * (" << lastResult[i].ToString() << ") ";
                    else
                        std::cout << "   " << "e_" << (i+1) << " * " << lastResult[i].ToString() << " ";
                    if (i != lastResult.Size()-1) std::cout << " + ";
                    std::cout << std::endl;
                }

                std::cout << "\033[0m";
            }
        private:
            Parser parser;

            TensorDatabase database;

            // Store the command to that was executed to create %
            std::string lastCmd;
            std::map<std::string, std::string> definition;
        };

    }
}
