#pragma once

#include <sstream>
#include <fstream>
#include <unordered_map>
#include <mutex>

#include <iostream>

#include <common/singleton.hpp>
#include <common/scope_guard.hpp>
#include <common/logger.hpp>
#include <tensor/expression.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

namespace Construction {
    namespace Tensor {

        // Forward declaration
        class ExpressionDatabase;

        class ExpressionDatabaseLocalReaderV1 {
        public:
            /**
                \brief Read an expression from a storage file

                \param filename
                \param pos
                \return Expression
             */
            Expression operator()(const std::string& filename, size_t pos) const {
                // Open file
                std::ifstream file (filename.c_str(), std::ios::binary);

                // Go to the position
                try {
                    file.seekg(pos);
                } catch (...) {
                    return Expression::Void();
                }

                // If something went wrong, return void expression
                if (!file.good()) return Expression::Void();

                // Read size
                size_t size;
                file.read(reinterpret_cast<char*>(&size), sizeof(size));

                // Read the string
                std::string content (size, ' ');
                file.read(&content[0], size);

                Construction::Logger::Debug("Read the compressed string ", content);

                // Decompress
                {
                    std::stringstream ss (content);
                    std::stringstream is;

                    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
                    out.push(boost::iostreams::gzip_decompressor());
                    out.push(ss);
                    boost::iostreams::copy(out, is);

                    content = is.str();
                }

                // Deserialize the expression
                std::unique_ptr<Expression> expression;
                try {
                    std::stringstream ss (content);
                    expression = Expression::Deserialize(ss);
                    if (!expression) return Expression::Void();
                } catch (...) {
                    return Expression::Void();
                }

                return *expression;
            }
        };

        class ExpressionDatabase : public Singleton<ExpressionDatabase> {
        public:
            void Initialize(const std::string& filename) {
                this->filename = filename;

                // Check if there is a temp file, if yes, rename to given filename.
                // This leads to the database always being restored automatically,
                // even if there was a crash inbetween resulting in the `filename`
                // to be broken and the temp file still being on the hard disk.
                {
                    bool tempExists = false;
                    {
                        std::ifstream file(filename + "temp");
                        tempExists = file.good();
                    }

                    if (tempExists) {
                        // Remove original file
                        std::remove(filename.c_str());

                        // Rename the temp file
                        std::rename((filename + "temp").c_str(), filename.c_str());
                    }
                }

                // Check if the file exists
                bool exists=false;
                {
                    std::ifstream file(filename);
                    exists = file.good();
                }

                if (exists) ReadKeysFromFile(filename);

                // Set initialized
                initialized = true;
            }
        public:
            void Deactivate() {
                active = false;
            }

            void Activate() {
                active = true;
            }
        private:
            void ReadKeysFromFile(const std::string& filename) {
                assert(initialized && "The expression database needs to be initialized first");

                // Open the file
                std::ifstream file(filename.c_str(), std::ios::binary);

                // Read the version
                unsigned MAJOR, MINOR, PATCH;

                try {
                    file.read(reinterpret_cast<char *>(&MAJOR), sizeof(MAJOR));
                    file.read(reinterpret_cast<char *>(&MINOR), sizeof(MINOR));
                    file.read(reinterpret_cast<char *>(&PATCH), sizeof(PATCH));
                } catch (...) {
                    return;
                }

                // TODO: version check
                Construction::Logger::Debug("Opened file ", filename);
                Construction::Logger::Debug("Version ", MAJOR, ".", MINOR, ".", PATCH);

                // Read number of entries
                size_t numDefs;
                try {
                    file.read(reinterpret_cast<char *>(&numDefs), sizeof(numDefs));
                } catch (...) {
                    return;
                }

                // Copy the old definition
                auto oldDefinitions = definitions;

                // Clear the content
                definitions.clear();

                Common::ScopeGuard guard ([&]() {
                    // Roll back
                    definitions = oldDefinitions;
                });

                // Read every entry
                for (int i=0; i<numDefs; ++i) {
                    try {
                        // Read size of the definition
                        size_t size;
                        file.read(reinterpret_cast<char*>(&size), sizeof(size));

                        // Read the definition
                        std::string definition(size, ' ');
                        file.read(&definition[0], size);

                        // Read the position in the file
                        size_t pos;
                        file.read(reinterpret_cast<char*>(&pos), sizeof(pos));

                        // Add the definition
                        definitions.insert({ definition, pos });

                        Construction::Logger::Debug("Found ", definition, " located at ", pos);
                    } catch (...) {
                        // If there was an error, return
                        return;
                    }
                }

                // Everything file, dismiss
                guard.Dismiss();
            }
        public:
            bool Contains(const std::string& name) const {
                assert(initialized && "The expression database needs to be initialized first");

                if (!active) return false;

                std::unique_lock<std::mutex> lock(mutex);

                auto it = definitions.find(name);
                return (it != definitions.end());
            }

            /**
                \brief Gets an expression from the database

                \param name     The name of the expression
                \returns The expression
             */
            Expression Get(const std::string& name) const {
                assert(initialized && "The expression database needs to be initialized first");

                // If not active, return void
                if (!active) return Expression::Void();

                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                // Check if the expression is in the database
                auto it = definitions.find(name);
                if (it == definitions.end()) return Expression::Void();

                // Look in the cache
                {
                    auto expr = Cache(name);
                    if (expr) return *expr;
                }

                return ReadExpressionFromFile(filename, it->second);
            }
        private:
            /**
                \brief Read an expression from a database file

                Reads an expression from a database file at the given position.
                In case something goes wrong, it returns Void.

                \param filename     The filename of the database
                \param pos          The position in the file
                \returns The expression
             */
            Expression ReadExpressionFromFile(const std::string& filename, size_t pos) const {
                // Open file
                std::ifstream file (filename.c_str(), std::ios::binary);

                // Go to the position
                try {
                    file.seekg(pos);
                } catch (...) {
                    return Expression::Void();
                }

                // If something went wrong, return void expression
                if (!file.good()) return Expression::Void();

                // Read size
                size_t size;
                file.read(reinterpret_cast<char*>(&size), sizeof(size));

                // Read the string
                std::string content (size, ' ');
                file.read(&content[0], size);

                Construction::Logger::Debug("Read the compressed string ", content);

                // Decompress
                {
                    std::stringstream ss (content);
                    std::stringstream is;

                    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
                    out.push(boost::iostreams::gzip_decompressor());
                    out.push(ss);
                    boost::iostreams::copy(out, is);

                    content = is.str();
                }

                // Deserialize the expression
                std::unique_ptr<Expression> expression;
                try {
                    std::stringstream ss (content);
                    expression = Expression::Deserialize(ss);
                    if (!expression) return Expression::Void();
                } catch (...) {
                    return Expression::Void();
                }

                return *expression;
            }
        public:
            void Cache(const std::string& definition, const Expression& expression) {
                auto it = std::find(cache_keys.begin(), cache_keys.end(), definition);

                // If this is already in the cache, delete it
                if (it != cache_keys.end()) {
                    cache_keys.erase(it);
                    cache_values.erase(cache_values.begin() + std::distance(cache_keys.begin(), it));
                }

                // Add the values
                cache_keys.push_back(definition);
                cache_values.push_back(expression);

                // If the value is larger than the maximal cache size, delete the first element
                if (cache_keys.size() > maximalCacheSize) {
                    cache_keys.erase(cache_keys.begin());
                    cache_values.erase(cache_values.begin());
                }
            }

            std::unique_ptr<Expression> Cache(const std::string& definition) const {
                auto it = std::find(cache_keys.begin(), cache_keys.end(), definition);
                if (it == cache_keys.end()) return nullptr;

                return std::unique_ptr<Expression>(new Expression(cache_values[std::distance(cache_keys.begin(), it)]));
            }
        public:
            /**
                \brief Insert the (definition, expression) tuple into the database

                Insert the (definition, expression) tuple into the database. The method
                is atomic and implements the transaction pattern, i.e. in case of an
                error while inserting, the database rolls back to the previous state.

                \param definition   The definition of the expression
                \param expression   The expression
                \returns Insertion was successful?
             */
            bool Insert(const std::string& definition, const Expression& expression) {
                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                // If not active, return
                if (!active) return false;

                // Copy the definitions for rollback
                std::unordered_map<std::string, size_t> oldDefinitions = definitions;

                // Insert the expression into the list of all definitions
                try {
                    auto it = definitions.find(definition);
                    bool definitionFound = it != definitions.end();
                    if (!definitionFound) {
                        // Already insert the element, but keep the position at zero, update later
                        auto pair = definitions.insert({definition, 0});

                        it = pair.first;

                        // If could not insert into the definition
                        if (!pair.second) return false;
                    }
                } catch (...) {
                    return false;
                }

                Common::ScopeGuard guard([&]() {
                    // Rollback to old definitions map
                    definitions = oldDefinitions;
                });

                // Copy the database into another file
                bool fileExisted=false;
                try {
                    {
                        std::ifstream inf(filename.c_str());
                        fileExisted = inf.good();
                    }

                    if (fileExisted) {
                        int err = std::rename(filename.c_str(), (filename + "temp").c_str());
                        if (err) throw std::runtime_error("Could not rename file");
                    }
                } catch (...) {
                    return false;
                }

                guard.Guard([&]() {
                    // Remove the file
                    std::remove(filename.c_str());

                    // Rename the file again
                    if (fileExisted) {
                        std::rename((filename + "temp").c_str(), filename.c_str());
                    }
                });

                // Write the file
                try {
                    std::ofstream file(filename, std::ios::binary);
                    size_t pos = 0;

                    // Write the format version
                    {
                        unsigned MAJOR = 1;
                        unsigned MINOR = 0;
                        unsigned PATCH = 0;

                        file.write(reinterpret_cast<const char *>(&MAJOR), sizeof(MAJOR));
                        file.write(reinterpret_cast<const char *>(&MINOR), sizeof(MINOR));
                        file.write(reinterpret_cast<const char *>(&PATCH), sizeof(PATCH));

                        pos += 3 * sizeof(unsigned);
                    }

                    // Write the number of entries
                    {
                        size_t size = definitions.size();
                        file.write(reinterpret_cast<const char *>(&size), sizeof(size));

                        pos += sizeof(size);
                    }

                    // Calculate size of the header
                    size_t headerSize = 0;
                    size_t headerPos = pos;

                    for (auto &defn : definitions) {
                        headerSize += sizeof(size_t);       // length of the string
                        headerSize += defn.first.size();    // the characters of the string
                        headerSize += sizeof(size_t);       // the position in the file
                    }

                    // Allocate empty space for the buffer
                    {
                        unsigned char buffer [headerSize];
                        file.write((char*) buffer, headerSize);
                    }

                    pos += headerSize;

                    // Write the expressions and update the positions
                    for (auto& it : definitions) {
                        Expression expr;

                        // If the definition is one of the old values ...
                        if (it.first != definition) {
                            // ... read data from saved database
                            expr = ReadExpressionFromFile(filename + "temp", it.second);
                        } else {
                            // ... else just use the existing expression
                            expr = expression;
                        }

                        // Update the position in the list of definitions
                        it.second = pos;

                        // Serialize the expression
                        std::string content;
                        {
                            std::stringstream ss;
                            expr.Serialize(ss);
                            content = ss.str();

                            std::stringstream os;

                            // Compress
                            boost::iostreams::filtering_streambuf <boost::iostreams::input> out;
                            out.push(boost::iostreams::gzip_compressor(boost::iostreams::gzip_params(boost::iostreams::gzip::best_compression)));
                            out.push(ss);
                            boost::iostreams::copy(out, os);

                            // To output
                            content = os.str();
                        }

                        // Write the expression size
                        {
                            size_t size = content.size();
                            file.write(reinterpret_cast<const char *>(&size), sizeof(size));
                            pos += sizeof(size);
                        }

                        // Write the expression
                        file << content;

                        // Increase the position
                        pos += content.size();
                    }

                    // Write the header
                    file.seekp(headerPos);

                    for (auto &pair : definitions) {
                        // Write the size of the definition
                        {
                            size_t size = pair.first.size();
                            file.write(reinterpret_cast<const char *>(&size), sizeof(size));
                        }

                        // Write the definition
                        file << pair.first;

                        // Write the position in the file
                        file.write(reinterpret_cast<const char *>(&pair.second), sizeof(pair.second));
                    }
                } catch (...) {
                    return false;
                }

                // Write the (definition,expression) into the cache
                Cache(definition, expression);

                guard.Guard([&]() {
                    cache_keys.erase(--cache_keys.end());
                    cache_values.erase(--cache_values.end());
                });

                // Everything went well, dismiss the guard
                guard.Dismiss();

                // Delete the old file
                try {
                    if (fileExisted) {
                        std::remove((filename + "temp").c_str());
                    }
                } catch (...) {

                }

                return true;
            }
        public:
            /**
                \brief Clear the database

                Clear the database. This also deletes everything that is stored
                on disk. Note, that one cannot undo this step!
             */
            void Clear() {
                // Remove the file from disk
                std::remove(filename.c_str());

                // Clear the definitions
                definitions.clear();

                // Clear cache
                cache_keys.clear();
                cache_values.clear();
            }

            /**
                \brief Return the size of the database
             */
            size_t Size() const {
                return definitions.size();
            }
        private:
            std::unordered_map<std::string, size_t> definitions;

            // Cache containers
            std::vector<std::string> cache_keys;
            std::vector<Expression> cache_values;

            std::string filename;

            unsigned maximalCacheSize = 128;

            mutable std::mutex mutex;

            bool initialized=false;
            bool active=true;
        };

    }
}
