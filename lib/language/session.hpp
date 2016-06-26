#pragma once

#include <string>
#include <sstream>
#include <map>
#include <fstream>

#include <common/singleton.hpp>

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

namespace Construction {
    namespace Language {

        class Session : public Singleton<Session> {
        public:
            Session() { }
        public:
            Expression GetCurrent() const {
                return current;
            }

            std::string GetLastCommandString() const {
                return lastCmd;
            }

            Notebook& GetNotebook() { return notebook; }

            void SetCurrent(const std::string& cmd, const Expression& tensors) {
                lastCmd = cmd;
                current = tensors;
            }

            Expression& Get(const std::string& name) {
                return memory[name];
            }

            Expression& operator[](const std::string& name) {
                return memory[name];
            }

            size_t Size() const { return memory.size(); }
        public:
            void SaveToFile(const std::string& filename) const {
                /*
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
                        size_t size = lastCmd.size();
                        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
                        os << lastCmd;
                    }

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

                file.close();*/
            }

            void LoadFromFile(const std::string& filename) {
                /*
                std::ifstream file (filename);
                std::stringstream is;

                // Clear
                notebook.Clear();
                current = Tensor::Tensor::Zero();
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
                    // Load last cmd
                    {
                        size_t size;
                        is.read(reinterpret_cast<char*>(&size), sizeof(size));

                        std::string string(size, ' ');
                        is.read(&string[0], size);

                        lastCmd = string;
                    }

                    // Load container data
                    std::string containerString;
                    {
                        size_t length;
                        is.read(reinterpret_cast<char*>(&length), sizeof(length));

                        std::string string (length, ' ');
                        is.read(&string[0], length);

                        containerString = string;
                    }

                    // Deserialize
                    {
                        std::shared_ptr<Tensor::Tensor> container;
            
                        std::stringstream ss (containerString);
                        container = Tensor::Tensor::Deserialize(ss);

                        current = *container;
                    }
                }

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
                        std::string containerString;
                        {
                            size_t length;
                            is.read(reinterpret_cast<char*>(&length), sizeof(length));

                            std::string string (length, ' ');
                            is.read(&string[0], length);

                            containerString = string;
                        }

                        // Deserialize
                        std::shared_ptr<Tensor::Tensor> container;
                        {
                            std::stringstream ss (containerString);
                            container = Tensor::Tensor::Deserialize(ss);
                        }

                        // Insert into memory
                        memory.insert({name, *container});
                    }
                }*/
            }
        private:
            Notebook notebook;

            std::string lastCmd;
            Expression current;
            std::map<std::string, Expression> memory;
        };

        /**
            \class SaveCommand


         */
        CLI_COMMAND(Save, false)
            std::string Help() const {
                return "Save(<String>)";
            }

            Expression Execute() const {
                auto filename = GetString(0);
                Session::Instance()->SaveToFile(filename);

                return Tensor::Tensor();
            }
        };

        REGISTER_COMMAND(Save);
        REGISTER_ARGUMENT(Save, 0, ArgumentType::STRING);

        /**
            
         */
        /**
            \class ExportCommand


         */
        CLI_COMMAND(Load, false)
            std::string Help() const {
                return "Load(<String>)";
            }

            Expression Execute() const {
                auto filename = GetString(0);
                Session::Instance()->LoadFromFile(filename);

                return Tensor::Tensor();
            }
        };

        REGISTER_COMMAND(Load);
        REGISTER_ARGUMENT(Load, 0, ArgumentType::STRING);

    }
}
