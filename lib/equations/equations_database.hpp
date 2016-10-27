#pragma once

#include <common/scope_guard.hpp>
#include <common/singleton.hpp>
#include <equations/equations.hpp>
#include <equations/coefficient.hpp>

namespace Construction {
    namespace Equations {

        // Forward declaration
        class EquationsProtocol;

        class EquationsProtocolStep {
        public:
            enum Type {
                COEFFICIENT = 1,
                EQUATION = 2,
                MERGE = 3
            };
        public:
            EquationsProtocolStep(EquationsProtocol& protocol, Type type);
        public:
            bool IsCoefficient() const { return type == COEFFICIENT; }
            bool IsEquation() const { return type == EQUATION; }
            bool IsMerge() const { return type == MERGE; }
        private:
            EquationsProtocol& protocol;
            Type type;

            friend class EquationsProtocol;
        };

        class ProtocolCoefficientStep : public EquationsProtocolStep {
        public:
            ProtocolCoefficientStep(EquationsProtocol& protocol, CoefficientReference ref) : EquationsProtocolStep(protocol), ref(ref) { }
        private:
            CoefficientReference ref;

            friend class EquationsProtocol;
        };

        class ProtocolEquationsStep : public EquationsProtocolStep {
        public:
            ProtocolEquationsStep(EquationsProtocol& protocol, const Equation& equation, const Tensor::Substitution& substitution) : EquationsProtocolStep(protocol), equation(eqation), substitution(substitution) { }
        private:
            Equation equation;
            Tensor::Substitution substitution;

            friend class EquationsProtocol;
        };

        class ProtocolMergeStep : public EquationsProtocolStep {
        public:
            ProtocolMergeStep(EquationsProtocol& protocol, const Equation& equation, const Tensor::Substitution& substitution) : EquationsProtocolStep(protocol), equation(eqation), substitution(substitution) { }
        private:
            Equation equation;
            Tensor::Substitution substitution;

            friend class EquationsProtocol;
        };

        /**
            \brief Store the result of an equation in a database
         */
        class EquationsProtocol {
        public:
            EquationsProtocol(const std::string& filename, std::vector<std::shared_ptr<Construction::Equations::Equation>> equations) : filename(filename) {
                // Register the protocol as observer for all coefficients
                for (auto& coeff : *Coefficients::Instance()) {
                    coeff.second->RegisterObserver(std::bind(&EquationsProtocol::FinishCoefficient, this));
                }

                // Register the protocol as observer for all equations
                for (auto& eq : equations) {
                    eq->RegisterObserver(std::bind(&EquationsProtocol::FinishEquation, this));
                }

                // Register the protocol as observer for merging steps
                SubstitutionManager::Instance()->RegisterObserver(std::bind(&EquationsProtocol::FinishMerge, this));
            }
        public:
            void FinishCoefficient(const CoefficientReference& reference) {
                ProtocolCoefficientStep step(*this, reference);
            }

            void FinishEquation(const Equation& equation) {
                ProtocolEquationsStep step(*this, equation, equation.GetSubstition());
            }

            void FinishMerge(const Substitution& substitution) {
                ProtocolMergeStep step(*this, substitution);
            }
        private:
            size_t GetNumberOfLogEntries(const std::string& filename = "") const {
                std::string f = this->filename;
                if (filename != "") f = filename;

                std::ifstream file (f.c_str());
                if (!file.good()) return 0;

                // Read version
                unsigned MAJOR, MINOR, PATCH;
                file.read(reinterpret_cast<char*>(&MAJOR), sizeof(MAJOR));
                file.read(reinterpret_cast<char*>(&MINOR), sizeof(MINOR));
                file.read(reinterpret_cast<char*>(&PATCH), sizeof(PATCH));

                size_t headerPos;
                file.read(reinterpret_cast<char*>(&headerPos), sizeof(headerPos));
                file.read(reinterpret_cast<char*>(&headerPos), sizeof(headerPos));

                // Jump to header pos
                file.seekg(headerPos);

                size_t entries;
                file.read(reinterpret_cast<char*>(&entries), sizeof(entries));

                return entries;
            }

            std::vector<std::pair<size_t, size_t>> ReadHeader(const std::string& filename) {
                std::ifstream file (f.c_str());
                if (!file.good()) return {};

                // Read version
                unsigned MAJOR, MINOR, PATCH;
                file.read(reinterpret_cast<char*>(&MAJOR), sizeof(MAJOR));
                file.read(reinterpret_cast<char*>(&MINOR), sizeof(MINOR));
                file.read(reinterpret_cast<char*>(&PATCH), sizeof(PATCH));

                size_t headerPos;
                file.read(reinterpret_cast<char*>(&headerPos), sizeof(headerPos));
                file.read(reinterpret_cast<char*>(&headerPos), sizeof(headerPos));

                // Jump to header pos
                file.seekg(headerPos);

                // Read the number of entries
                size_t entries;
                file.read(reinterpret_cast<char*>(&entries), sizeof(entries));

                std::vector<std::pair<size_t, size_t>> result;

                for (int i=0; i<entries; ++i) {
                    // Read pos
                    size_t pos;
                    file.read(reinterpret_cast<char*>(&pos), sizeof(pos));

                    // Store the current position
                    auto current = file.tellg();

                    // Jump into the definition of the log entry
                    file.seekg(pos);

                    // Read the size
                    size_t size;
                    file.read(reinterpret_cast<char*>(&size), sizeof(size));

                    // Jump back to the beginning
                    file.seekg(current);

                    result.push_back({ pos, size });
                }

                return result;
            }

            void DumpBody(const std::string& filename, std::ofstream& file) {
                std::ifstream in (filename.c_str());

                // If not exists, return
                if (!filename.good()) return;

                // Read version
                unsigned MAJOR, MINOR, PATCH;
                in.read(reinterpret_cast<char*>(&MAJOR), sizeof(MAJOR));
                in.read(reinterpret_cast<char*>(&MINOR), sizeof(MINOR));
                in.read(reinterpret_cast<char*>(&PATCH), sizeof(PATCH));

                size_t headerPos;
                in.read(reinterpret_cast<char*>(&headerPos), sizeof(headerPos));
                in.read(reinterpret_cast<char*>(&headerPos), sizeof(headerPos));
                in.read(reinterpret_cast<char*>(&headerPos), sizeof(headerPos));

                // Go to the body
                in.seekp(headerPos);

                file << in.rdbuf();
            }
        private:
            void WriteCoefficient(const CoefficientReference& ref, std::ofstream& file, size_t& pos) {
                // Write the definition
                unsigned l = step.ref->GetNumberOfLeftIndices();
                unsigned r = step.ref->GetNumberOfRightIndices();
                unsigned ld = step.ref->GetNumberOfLeftDerivativeIndices();
                unsigned rd = step.ref->GetNumberOfRightDerivativeIndices();

                file.write(reinterpret_cast<const char*>(&l), sizeof(l));
                file.write(reinterpret_cast<const char*>(&l), sizeof(ld));
                file.write(reinterpret_cast<const char*>(&l), sizeof(r);
                file.write(reinterpret_cast<const char*>(&l), sizeof(rd));

                pos += sizeof(l) + sizeof(ld) + sizeof(r) + sizeof(rd);

                // Serialize the tensor
                std::string content;
                {
                    std::stringstream ss;
                    step.ref->GetAsync().Serialize(ss);
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

            void DumpCoefficient(const ProtocolCoefficientStep& step, std::ofstream& file, size_t& pos) {
                // Write the type
                unsigned type = 7;
                file.write(reinterpret_cast<const char*>(&type), sizeof(type));

                pos += sizeof(type);

                // Write the coefficient
                WriteCoefficient(step.ref, file, pos);
            }

            void DumpEquation(const ProtocolEquationsStep& step, std::ofstream& file, size_t& pos) {
                // Write the type
                unsigned type = 23;
                file.write(reinterpret_cast<const char*>(&type), sizeof(type));

                pos += sizeof(type);

                // Serialize the equation code again
                {
                    auto code = step.equation.GetCode();

                    size_t size = code.size();
                    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

                    pos += sizeof(size);

                    file << code;

                    pos += size;
                }

                // Serialize the substitution
                {
                    std::string content;
                    {
                        std::stringstream ss;
                        step.equation.GetSubstition().Serialize(ss);
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

                    size_t size = content.size();
                    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

                    pos += sizeof(size);

                    file << content;

                    pos += size;
                }

                // Dump ALL coefficients
                {
                    CoefficientsLock coeffsLock;

                    size_t size = Coefficients::Instance()->Size();
                    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
                    pos += sizeof(size);

                    for (auto it = Coefficients::Instance()->begin(); it != Coefficients::Instance()->end(); ++it) {
                        WriteCoefficient(it->second, file, pos);
                    }
                }
            }

            void DumpMerge(const ProtocolMergeStep& step, std::ofstream& file, size_t& pos) {
                // Write the type
                unsigned type = 42;
                file.write(reinterpret_cast<const char*>(&type), sizeof(type));

                pos += sizeof(type);

                // Serialize the substitution
                {
                    std::string content;
                    {
                        std::stringstream ss;
                        step.substitution.Serialize(ss);
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

                    size_t size = content.size();
                    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

                    pos += sizeof(size);

                    file << content;

                    pos += size;
                }
            }

            bool WriteStep(const EquationsProtocolStep& step) {
                std::unique_lock<std::mutex> lock(mutex);

                // Rename the original file
                bool fileExisted=false;
                try {
                    {
                        std::ifstream file (filename.c_str());
                        fileExisted = file.good();
                    }

                    if (fileExisted) {
                        int err = std::rename(filename.c_str(), (filename + "temp").c_str());
                        if (err) throw std::runtime_error("Could not rename file");
                    }
                } catch (...) {
                    return false;
                }

                // Create a new file
                std::ofstream file (filename.c_str());

                Common::ScopeGuard guard([&]() {
                    // Remove the file
                    std::remove(filename.c_str());

                    // Rename the file again
                    if (fileExisted) {
                        std::rename((filename + "temp").c_str(), filename.c_str());
                    }
                });

                size_t headerPos = 0;
                size_t pos = 0;

                size_t newEntryPosition;

                // Write header
                {
                    unsigned MAJOR = 1;
                    unsigned MINOR = 0;
                    unsigned PATCH = 0;

                    file.write(reinterpret_cast<const char*>(&MAJOR), sizeof(MAJOR));
                    file.write(reinterpret_cast<const char*>(&MINOR), sizeof(MINOR));
                    file.write(reinterpret_cast<const char*>(&PATCH), sizeof(PATCH));

                    pos += 3 * sizeof(MAJOR);

                    // Write a dummy block that stores the jump positions for the different parts
                    file.write(reinterpret_cast<const char*>(&pos), sizeof(pos));
                    file.write(reinterpret_cast<const char*>(&pos), sizeof(pos));
                    file.write(reinterpret_cast<const char*>(&pos), sizeof(pos));

                    pos += sizeof(pos);
                    auto currentHeaderPos = pos;
                    pos += 2 * sizeof(pos);

                    // Write the equations
                    size_t numEquations = equations.size();
                    file.write(reinterpret_cast<const char*>(&numEquations), sizeof(numEquations));
                    pos += sizeof(numEquations);

                    for (int i=0; i<numEquations; ++i) {
                        auto code = equations[i]->GetCode();
                        size_t size = code.size();

                        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
                        pos += sizeof(size);

                        file << code;
                        pos += size;
                    }

                    // Go back to dummy header and write the pos
                    {
                        auto currentPos = pos;

                        file.seekp(currentHeaderPos);
                        file.write(reinterpret_cast<const char *>(&currentPos), sizeof(currentPos));
                        file.seekp(currentPos);
                    }

                    currentHeaderPos += sizeof(pos);

                    // Write the number of entries
                    size_t numEntries = GetNumberOfLogEntries((filename + "temp").c_str()) + 1;
                    file.write(reinterpret_cast<const char*>(&numEntries), sizeof(numEntries));
                    pos += sizeof(numEntries);

                    // Read the header
                    if (fileExisted) {
                        auto header = ReadHeader((filename + "temp").c_str());

                        for (auto &pair : header) {
                            // The position is shifted because the header grew by one entry
                            auto shifted = pair.first + sizeof(pair.first) + sizeof(pair.second);

                            // Write the new entries
                            file.write(reinterpret_cast<const char *>(&shifted), sizeof(shifted));
                            file.write(reinterpret_cast<const char *>(&pair.second), sizeof(pair.second));

                            pos += sizeof(shifted) + sizeof(pair.second);
                        }
                    }

                    newEntryPosition = pos;

                    // Allocate memory for the new entry in the header
                    {
                        size_t newEntryPos;
                        size_t newEntrySize;

                        // Update the position in the end
                        file.write(reinterpret_cast<const char *>(&newEntryPos), sizeof(newEntryPos));
                        file.write(reinterpret_cast<const char *>(&newEntrySize), sizeof(newEntrySize));

                        pos += sizeof(newEntryPos) + sizeof(newEntrySize);
                    }

                    // Update dummy header
                    currentPos = pos;
                    file.seekp(currentHeaderPos);
                    file.write(reinterpret_cast<const char*>(&currentPos), sizeof(currentPos));
                    file.seekp(currentPos);

                    // Store the position of the header
                    headerPos = pos;
                }

                // Dump the body of the old file
                DumpBody(filename + "temp", file, pos);

                // Append the new protocol entry
                size_t newElementPos = pos;

                if (step.IsCoefficient()) {
                    DumpCoefficient(step, file, pos);
                } else if (step.IsEquation()) {
                    DumpEquation(step, file, pos);
                } else if (step.IsMerge()) {
                    DumpMerge(step, file, pos);
                }

                // Calcualte the new element size
                size_t newElementSize = pos - newElementPos;

                // Jump back
                file.seekp(newEntryPosition);
                file.write(reinterpret_cast<const char*>(&newElementPos), sizeof(newElementPos));
                file.write(reinterpret_cast<const char*>(&newElementSize), sizeof(newElementSize));

                // Everything file, dismiss the guard
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

            friend class EquationsProtocolStep;
        private:
            std::string filename;

            mutable std::mutex mutex;

            std::vector<std::shared_ptr<Construction::Equations::Equation>> equations;
        };

    }
}