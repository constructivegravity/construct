#pragma once

#include <sstream>
#include <fstream>

#include <tensor/tensor.hpp>
#include <tensor/tensor_container.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

namespace Construction {
    namespace Tensor {

        class TensorDatabase {
        public:
            TensorDatabase() = default;
        public:
            void Clear() {
                data.clear();
            }

            size_t Size() const {
                return data.size();
            }
        public:
            bool Contains(const std::string& name) const {
                auto it = data.find(name);
                if (it == data.end()) return false;
                return true;
            }

            TensorContainer operator[](const std::string& name) const {
                auto it = data.find(name);
                if (it == data.end()) return TensorContainer();
                return it->second;
            }

            TensorContainer& operator[](const std::string& name) {
                return data[name];
            }

            std::map<std::string, TensorContainer>::iterator begin() {
                return data.begin();
            }

            std::map<std::string, TensorContainer>::iterator end() {
                return data.end();
            }

            std::map<std::string, TensorContainer>::const_iterator begin() const {
                return data.begin();
            }

            std::map<std::string, TensorContainer>::const_iterator end() const {
                return data.end();
            }
        public:
            void SaveToFile(const std::string& filename) const {
                std::stringstream os;

                // Write number of elements
                size_t size = data.size();
                os.write(reinterpret_cast<const char*>(&size), sizeof(size));

                // Iterate over all elements
                for (auto it = data.begin(); it != data.end(); it++) {
                    // Write the string
                    size = it->first.size();
                    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
                    os << it->first;

                    // Serialize the tensor container
                    {
                        std::stringstream ss;
                        it->second.Serialize(ss);

                        size = ss.str().size();
                        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
                        os << ss.str();
                    }
                }

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

            void LoadFromFile(const std::string& filename) {
                data.clear();

                std::ifstream file (filename);
                std::stringstream is;

                // Decompress
                {
                    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
                    out.push(boost::iostreams::gzip_decompressor());
                    out.push(file);
                    boost::iostreams::copy(out, is);
                }

                // Write number of elements
                size_t numElements;
                is.read(reinterpret_cast<char*>(&numElements), sizeof(numElements));

                std::cout << numElements << std::endl;

                // Iterate over all elements
                for (size_t i=0; i<numElements; i++) {
                    // Read name
                    std::string name;
                    {
                        size_t length;
                        is.read(reinterpret_cast<char*>(&length), sizeof(length));

                        std::string string (length, ' ');
                        is.read(&string[0], length);

                        name = string;
                    }

                    // Load tensor container data
                    std::string containerString;
                    {
                        size_t length;
                        is.read(reinterpret_cast<char*>(&length), sizeof(length));

                        std::string string (length, ' ');
                        is.read(&string[0], length);

                        containerString = string;
                    }

                    // Deserialize the container string
                    std::shared_ptr<TensorContainer> container;
                    {
                        std::stringstream ss (containerString);
                        container = TensorContainer::Deserialize(ss);
                    }

                    // Insert the pair
                    data.insert({ name, *container });
                }

                file.close();
            }
        private:
            std::map<std::string, TensorContainer> data;
        };

    }
}