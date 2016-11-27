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

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace Construction {
    namespace Language {

        class WrongTypeException : public Exception {
        public:
            WrongTypeException() : Exception("Unexpected type error") { }
        };

        class IncompatibleTypesException : public Exception {
        public:
            IncompatibleTypesException() : Exception("Incompatible types") { }
        };

        class CLI {
        public:
            CLI() {
                crashFile = ".crashfile";
            }

            CLI(int argc, char** argv) {
                /*auto crashFile = boost::filesystem::system_complete(argv[0]).remove_filename().string();
                if (crashFile[crashFile.size()-1] != '/') crashFile += "/";
                crashFile += ".crashfile";*/
                crashFile = ".crashfile";
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
                            if (it == definition.end()) result.append("");
                            else result.append(it->second);
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

            std::string ToLaTeX(const std::shared_ptr<Node>& document) const {
                if (document->IsPrevious()) return "%";
                else if (document->IsLiteral()) {
                    return std::dynamic_pointer_cast<LiteralNode>(document)->GetText();
                } else if (document->IsNumeric()) {
                    return std::dynamic_pointer_cast<NumericNode>(document)->GetText();
                } else if (document->IsIndices()) {
                    IndexArgument arg(std::dynamic_pointer_cast<IndicesNode>(document)->GetText());
                    return arg.GetIndices().ToCommand();
                } else if (document->IsString()) {
                    return std::dynamic_pointer_cast<StringNode>(document)->GetText();
                } else if (document->IsBinary()) {
                    auto lhs = ToLaTeX(std::dynamic_pointer_cast<BinaryNode>(document)->GetLeft());
                    auto rhs = ToLaTeX(std::dynamic_pointer_cast<BinaryNode>(document)->GetRight());

                    return lhs + std::string(1, std::dynamic_pointer_cast<BinaryNode>(document)->GetOperator()) + rhs;
                } else if (document->IsCommand()) {
                    auto commandName = std::dynamic_pointer_cast<CommandNode>(document)->GetIdentifier()->GetText();

                    CommandPointer command;
                    std::vector<std::string> texedArgs;

                    // Create a instance of the command
                    try {
                        command = CommandManagement::Instance()->CreateCommand(commandName);
                    } catch (UnknownCommandException& err) {
                        Error("I do not know this command");
                        return Expression::Void();
                    }

                    // Turn the arguments into LaTeX code
                    auto args = std::dynamic_pointer_cast<CommandNode>(document)->GetArguments();

                    for (auto& arg : *args) {
                        texedArgs.push_back(ToLaTeX(arg));
                    }

                    // Return the output
                    std::string result = command->ToLaTeX(texedArgs);
                    return result;
                } else if (document->IsAssignment()) {
                    auto id = std::dynamic_pointer_cast<AssignmentNode>(document)->GetIdentifier()->GetText();
                    auto expression = ToLaTeX(std::dynamic_pointer_cast<AssignmentNode>(document)->GetExpression());

                    return id + " = " + expression;
                }

                return "";
            }

            Expression Execute(const std::shared_ptr<Node>& document, bool silent=false) {
                // Copy the most recent expression;
                Expression lastResult = Session::Instance()->GetCurrent();
                Expression previousResult = lastResult;

                Common::TimeMeasurement time;

                // Previous token
                if (document->IsPrevious()) {
                    return lastResult;
                }
                // Literals
                else if (document->IsLiteral()) {
                    auto id = std::dynamic_pointer_cast<LiteralNode>(document)->GetText();
                    lastResult = Session::Instance()->Get(id);
                    Session::Instance()->SetCurrent(definition[id], lastResult);
;
                    return lastResult;
                }
                // Numerics
                else if (document->IsNumeric()) {
                    auto str = std::dynamic_pointer_cast<NumericNode>(document)->GetText();

                    if (str.find(".") == std::string::npos) {
                        lastResult = Scalar::Fraction(std::atoi(str.c_str()), 1);
                    } else {
                        lastResult = Scalar::Fraction(std::atof(str.c_str()));
                    }

                    Session::Instance()->SetCurrent("", lastResult);

                    return lastResult;
                }
                // Indices
                else if (document->IsIndices()) {
                    // Use index arguments to parse the indices
                    IndexArgument arg(std::dynamic_pointer_cast<IndicesNode>(document)->GetText());

                    // Get the indices
                    lastResult = arg.GetIndices();

                    // Update the session
                    Session::Instance()->SetCurrent("", lastResult);

                    return lastResult;
                }
                // String
                else if (document->IsString()) {
                    // Get the value
                    lastResult = Tensor::StringExpression(std::dynamic_pointer_cast<StringNode>(document)->GetText());

                    // Update the session
                    Session::Instance()->SetCurrent("", lastResult);

                    return lastResult;
                }
                // Binary operations
                else if (document->IsBinary()) {
                    auto lhs = std::dynamic_pointer_cast<BinaryNode>(document)->GetLeft();
                    auto rhs = std::dynamic_pointer_cast<BinaryNode>(document)->GetRight();

                    // Execute the left command
                    auto leftResult = Execute(lhs, true);

                    // Reset the current instance to get the same initial conditions
                    Session::Instance()->SetCurrent("", lastResult);

                    // Execute the right command
                    auto rightResult = Execute(rhs, true);

                    // Do add them
                    switch (std::dynamic_pointer_cast<BinaryNode>(document)->GetOperator()) {
                        case '+':
                            if (leftResult.GetType() != rightResult.GetType()) {
                                throw IncompatibleTypesException();
                            }

                            if (leftResult.IsTensor()) {
                                lastResult = leftResult.As<Tensor::Tensor>() + rightResult.As<Tensor::Tensor>();
                            } else if (leftResult.IsScalar()) {
                                lastResult = leftResult.As<Tensor::Scalar>() + rightResult.As<Tensor::Scalar>();
                            } else if (leftResult.IsSubstitution()) {
                                lastResult = Tensor::Substitution::Merge({ leftResult.As<Tensor::Substitution>(), rightResult.As<Tensor::Substitution>() });
                            }
                            break;
                        case '-':
                            if (leftResult.GetType() != rightResult.GetType()) {
                                throw IncompatibleTypesException();
                            }

                            if (leftResult.IsTensor()) {
                                lastResult = leftResult.As<Tensor::Tensor>() - rightResult.As<Tensor::Tensor>();
                            } else if (leftResult.IsScalar()) {
                                lastResult = leftResult.As<Tensor::Scalar>() - rightResult.As<Tensor::Scalar>();
                            }
                            break;
                        case '*':
                            if (!(leftResult.IsTensor() && rightResult.IsTensor()) &&
                                !(leftResult.IsTensor() && rightResult.IsScalar()) &&
                                !(leftResult.IsScalar() && rightResult.IsTensor()) &&
                                !(leftResult.IsScalar() && rightResult.IsScalar())
                            ) {
                                throw IncompatibleTypesException();
                            }

                            if (leftResult.IsTensor()) {
                                Expression newExpression = (rightResult.IsTensor()) ? leftResult.As<Tensor::Tensor>() * rightResult.As<Tensor::Tensor>() : leftResult.As<Tensor::Tensor>() * rightResult.As<Tensor::Scalar>();

                                // Convert completely contracted tensors (a.k.a. scalars) into real scalars
                                if (newExpression.IsTensor() && newExpression.As<Tensor::Tensor>().IsScalar()) {
                                    newExpression = (*newExpression.As<Tensor::Tensor>().As<Tensor::ScalarTensor>())();
                                }

                                lastResult = newExpression;
                            } else if (leftResult.IsScalar()) {
                                auto newExpression = Expression::Void();

                                if (rightResult.IsScalar()) {
                                    newExpression = leftResult.As<Tensor::Scalar>() * rightResult.As<Tensor::Scalar>();
                                } else {
                                    Tensor::Tensor multiplied = rightResult.As<Tensor::Tensor>() * leftResult.As<Tensor::Scalar>();
                                    newExpression = multiplied;
                                }

                                lastResult = newExpression;
                            }
                            break;
                    }

                    Session::Instance()->SetCurrent("", lastResult);

                    return lastResult;

                } else if (document->IsNegation()) {
                    auto node = std::dynamic_pointer_cast<NegationNode>(document)->GetNode();

                    // Execute the command and load the latest content to memory
                    auto last = Execute(node, true);

                    if (last.IsScalar()) {
                        lastResult = -last.As<Tensor::Scalar>();
                    } else if (last.IsTensor()) {
                        lastResult = -last.As<Tensor::Tensor>();
                    }

                    // Update the value
                    Session::Instance()->SetCurrent("", lastResult);

                    return lastResult;
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
                        return Expression::Void();
                    }

                    // Apply arguments
                    auto args = std::dynamic_pointer_cast<CommandNode>(document)->GetArguments();

                    for (auto& arg : *args) {
                        // Execute again to load this
                        auto expr = Execute(arg, true);

                        // Set the last expression back
                        Session::Instance()->SetCurrent("", lastResult);

                        // Turn the argument into an argument
                        switch (expr.GetType()) {
                            case Tensor::ExpressionType::TENSOR: {
                                auto newArg = std::make_shared<TensorArgument>();
                                newArg->SetTensor(expr.As<Tensor::Tensor>());
                                command->AddArgument(std::move(newArg));
                            }
                                break;

                            case Tensor::ExpressionType::SCALAR: {
                                auto newArg = std::make_shared<NumericArgument>(expr.As<Tensor::Scalar>());
                                command->AddArgument(std::move(newArg));
                            }
                                break;

                            case Tensor::ExpressionType::SUBSTITUTION: {
                                auto newArg = std::make_shared<SubstitutionArgument>();
                                newArg->SetSubstitution(expr.As<Tensor::Substitution>());
                                command->AddArgument(std::move(newArg));
                            }
                                break;

                            case Tensor::ExpressionType::INDICES: {
                                auto newArg = std::make_shared<IndexArgument>(expr.As<Tensor::Indices>());
                                command->AddArgument(std::move(newArg));
                            }
                                break;

                            case Tensor::ExpressionType::STRING: {
                                auto newArg = std::make_shared<StringArgument>(expr.As<Tensor::StringExpression>().ToString());
                                command->AddArgument(std::move(newArg));
                            }
                                break;

                            default:
                                // TODO: proper exception
                                throw UnknownCommandException();
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

                    return lastResult;
                } else if (document->IsAssignment()) {
                    auto id = std::dynamic_pointer_cast<AssignmentNode>(document)->GetIdentifier()->GetText();
                    auto expression = std::dynamic_pointer_cast<AssignmentNode>(document)->GetExpression();

                    lastResult = Execute(expression, true);

                    // Store the variable in memory
                    Session::Instance()->Set(id, lastResult);

                    definition[id] = lastCmd;
                    //database[lastCmd] = lastResult;

                    // Print tensors unless in silent mode
                    /*if (!silent) {
                        PrintExpression(lastResult);

                        time.Stop();

                        // Print the required time
                        std::cout << "\033[90m   " << time << "\033[0m" << std::endl;
                    }*/

                    return lastResult;
                } else {
                    Error("Cannot execute this :'(");
                }
            }
        public:
            std::string ToLaTeX(const std::string& code) {
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
                    return "";
                }

#if RECOVER_FROM_EXCEPTIONS == 1
                try {
#endif
                    return ToLaTeX(document);
#if RECOVER_FROM_EXCEPTIONS == 1
                } catch(...) {

                }
#endif
                return "";
            }

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
                    Common::TimeMeasurement time;

                    auto lastResult = Execute(document, silent);
                    Session::Instance()->GetNotebook().Append(code);

                    time.Stop();

                    // Print tensors unless in silent mode
                    if (!silent) {
                        PrintExpression(lastResult);

                        // Print the required time
                        std::cout << "\033[90m   " << time << "\033[0m" << std::endl;
                    }

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
