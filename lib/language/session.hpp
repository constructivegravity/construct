#pragma once

#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <mutex>

#include <common/singleton.hpp>
#include <common/error.hpp>

#include <tensor/expression.hpp>
#include <language/notebook.hpp>

#include <language/command.hpp>
#include <language/argument.hpp>
#include <language/api.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

using Construction::Tensor::TensorContainer;
using Construction::Tensor::Expression;
using Construction::Exception;

namespace Construction {
    namespace Language {

        class CannotOpenSessionException : public Exception {
        public:
            CannotOpenSessionException() : Exception("Cannot open the session.") { }
        };

        class Session : public Singleton<Session> {
        public:
            Session() : current(Expression::Void()) { }
        public:
            Expression GetCurrent() const {
                Expression result;

                // Scope based locking
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    result = current;
                }

                return result;
            }

            std::string GetLastCommandString() const {
                return lastCmd;
            }

            Notebook& GetNotebook() { return notebook; }

            void SetCurrent(const std::string& cmd, const Expression& tensors) {
                std::unique_lock<std::mutex> lock(mutex);

                lastCmd = cmd;
                current = tensors;
            }

            /*Expression& Get(const std::string& name) {
                return memory[name];
            }*/

            Expression Get(const std::string& name) const {
                std::unique_lock<std::mutex> lock(mutex);

                auto it = memory.find(name);

                if (it == memory.end()) {
                    return Expression::Void();
                } else {
                    return it->second;
                }
            }

            void Set(const std::string& name, const Expression& expression) {
                std::unique_lock<std::mutex> lock(mutex);

                memory[name] = expression;
            }

            void Set(const std::string& name, Expression&& expression) {
                std::unique_lock<std::mutex> lock(mutex);

                memory[name] = std::move(expression);
            }

            void TurnCurrentIntoAVariable(const std::string& name) {
                std::unique_lock<std::mutex> lock(mutex);

                auto it = memory.find(name);
                if (it == memory.end()) {
                    memory.insert({ name, current });
                } else {
                    it->second = current;
                }
            }

            Expression& operator[](const std::string& name) {
                return memory[name];
            }

            size_t Size() const { return memory.size(); }
        public:
            void SaveToFile(const std::string& filename) const {
                std::unique_lock<std::mutex> lock(mutex);

                std::stringstream os;

                // Store the notebook
                {
                    size_t size = notebook.Size();
                    os.write(reinterpret_cast<const char*>(&size), sizeof(size));

                    for (auto& line : notebook) {
                        // Write line length
                        size_t size = line.size();
                        os.write(reinterpret_cast<const char*>(&size), sizeof(size));

                        // Write the characters
                        os << line;
                    }
                }

                // Store the current tensors
                {
                    // Store last command string
                    {
                        std::stringstream ss;
                        current.Serialize(ss);

                        size_t size = ss.str().size();
                        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
                        os << ss.str();
                    }
                }

                // Store the memory
                {
                    size_t size = memory.size();
                    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
                }

                for (auto it = memory.begin(); it != memory.end(); ++it) {
                    // Write the name of the variable
                    {
                        size_t size = it->first.size();
                        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
                        os << it->first;
                    }

                    // Serialize the tensor container
                    {
                        std::stringstream ss;
                        it->second.Serialize(ss);

                        size_t size = ss.str().size();
                        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
                        os << ss.str();
                    }
                }

                // Output file
                std::ofstream file (filename);

                // Compress
                {
                    boost::iostreams::filtering_streambuf <boost::iostreams::input> out;
                    out.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params(boost::iostreams::gzip::best_compression)));
                    out.push(os);
                    boost::iostreams::copy(out, file);
                }

                file.close();
            }

            /** 
                \brief Load a session from a file

                Load a session from a file. It reads all the executed command lines and restores
                the expressions on the stack and in the variable memory.

                \param  {std::string}    The filename

                \throws CannotOpenSessionException
             */
            void LoadFromFile(const std::string& filename) {
                std::unique_lock<std::mutex> lock(mutex);

                std::ifstream file (filename);
                std::stringstream is;

                // Clear
                notebook.Clear();
                current = Expression::Void();
                memory.clear();

                // Decompress
                {
                    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
                    out.push(boost::iostreams::gzip_decompressor());
                    out.push(file);
                    boost::iostreams::copy(out, is);
                }

                // Read notebook
                {
                    size_t numLines;
                    is.read(reinterpret_cast<char*>(&numLines), sizeof(numLines));

                    for (int i=0; i<numLines; ++i) {
                        size_t size;
                        is.read(reinterpret_cast<char*>(&size), sizeof(size));

                        std::string string (size, ' ');
                        is.read(&string[0], size);

                        notebook.Append(string);
                        std::cout << "> " << string << std::endl;
                    }
                }

                // Read the most recent result
                {
                    // Load container data
                    std::string expressionString;
                    {
                        size_t length;
                        is.read(reinterpret_cast<char*>(&length), sizeof(length));

                        std::string string (length, ' ');
                        is.read(&string[0], length);

                        expressionString = string;
                    }

                    // Deserialize
                    {
                        std::stringstream ss(expressionString);

                        std::unique_ptr<Expression> expression = Tensor::Expression::Deserialize(ss);

                        if (!expression) throw CannotOpenSessionException();

                        current = *expression;
                    }
                }

                // Print the most recent result
                {
                    std::stringstream ss;
                    ss << current.ToString();
                    std::string line;

                    // Change the output color
                    std::cout << "\033[" << current.GetColorCode() << "m";

                    // Shift the output by three characters
                    while (std::getline(ss, line)) {
                        std::cout << "   " << line << std::endl;
                    }
                }
                
                // Change the color back
                std::cout << "\033[0m";

                // Read the memory
                {
                    size_t size;
                    is.read(reinterpret_cast<char*>(&size), sizeof(size));

                    for (int i=0; i<size; i++) {
                        std::string name;

                        // Read variable name
                        {
                            size_t size;
                            is.read(reinterpret_cast<char*>(&size), sizeof(size));

                            std::string string (size, ' ');
                            is.read(&string[0], size);

                            name = string;
                        }

                        // Read the container
                        std::string expressionString;
                        {
                            size_t length;
                            is.read(reinterpret_cast<char*>(&length), sizeof(length));

                            std::string string (length, ' ');
                            is.read(&string[0], length);

                            expressionString = string;
                        }

                        // Deserialize
                        std::unique_ptr<Tensor::Expression> expression;
                        {
                            std::stringstream ss (expressionString);
                            expression = Tensor::Expression::Deserialize(ss);

                            if (!expression) throw CannotOpenSessionException();
                        }

                        // Insert into memory
                        memory.insert({name, *expression});
                    }
                }
            }
        private:
            Notebook notebook;

            mutable std::mutex mutex;
            std::string lastCmd;
            Expression current;
            std::map<std::string, Expression> memory;
        };

        /**
            \class SaveCommand


         */
        CLI_COMMAND(SaveSession)
            std::string Help() const {
                return "SaveSession(<String>)";
            }

            Expression Execute() const {
                auto filename = GetString(0);
                Session::Instance()->SaveToFile(filename);

                return Expression::Void();
            }
        };

        REGISTER_COMMAND(SaveSession);
        REGISTER_ARGUMENT(SaveSession, 0, ArgumentType::STRING);

        /**
            
         */
        /**
            \class ExportCommand


         */
        CLI_COMMAND(LoadSession)
            std::string Help() const {
                return "LoadSession(<String>)";
            }

            Expression Execute() const {
                auto filename = GetString(0);
                Session::Instance()->LoadFromFile(filename);

                return Expression::Void();
            }
        };

        REGISTER_COMMAND(LoadSession);
        REGISTER_ARGUMENT(LoadSession, 0, ArgumentType::STRING);

    }
}