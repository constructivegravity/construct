#pragma once

#include <unordered_map>
#include <mutex>
#include <thread>
#include <functional>
#include <random>
#include <memory>
#include <algorithm>
#include <string>

#include <common/uuid.hpp>
#include <language/session.hpp>
#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/expression_database.hpp>
#include <language/api.hpp>

using Construction::Common::Unique;
using Construction::Language::Session;
using Construction::Tensor::Tensor;
using Construction::Tensor::Indices;

namespace Construction {
    namespace Equations {

        struct CoefficientDefinition {
            enum class SymmetryType {
                SYMMETRIC = 0,
                ANTISYMMETRIC = 1,
                NONE = 2
            };

            struct Block {
                unsigned int indices;
                unsigned int derivatives;
                SymmetryType symmetry;

                bool operator<(const Block& other) const {
                    if (indices < other.indices) return true;
                    else if (indices > other.indices) return false;

                    if (derivatives < other.derivatives) return true;
                    else if (derivatives > other.derivatives) return false;

                    return symmetry < other.symmetry;
                }

                bool operator<=(const Block& other) const {
                    if (indices < other.indices) return true;
                    else if (indices > other.indices) return false;

                    if (derivatives < other.derivatives) return true;
                    else if (derivatives > other.derivatives) return false;

                    return symmetry <= other.symmetry;
                }

                bool operator==(const Block& other) const {
                    return indices == other.indices && derivatives == other.derivatives && symmetry == other.symmetry;
                }

                inline bool operator!=(const Block& other) const {
                    return !(*this == other);
                }

                inline bool operator>(const Block& other) const {
                    return other < *this;
                }

                inline bool operator>=(const Block& other) const {
                    return other <= *this;
                }
            };

            void AddBlock(unsigned int indices, unsigned derivatives, SymmetryType symmetry) {
                blocks.push_back(Block{indices, derivatives, symmetry});
            }

            size_t TotalIndices() const {
                return indices.Size();
            }

            void Canonicalize() {
                /*using sort_t = std::tuple<Block, Indices, int>;
                std::vector<sort_t> data;
                Indices old = indices;

                /*std::cout << ToString() << std::endl;

                for (int i=0; i<blocks.size(); ++i) {
                    auto ind = old.Partial({ 0, blocks[i].indices + blocks[i].derivatives-1 });
                    old = old.Partial({ blocks[i].indices + blocks[i].derivatives, static_cast<unsigned int>(old.Size())-1 });

                    data.push_back(std::make_tuple(blocks[i], ind, i));
                }

                // Sort
                std::sort(data.begin(), data.end(), [&](const sort_t& a, const sort_t& b) {
                    //if (!this->exchangeSymmetries[std::get<2>(a)]) return true;
                    return std::get<0>(a) < std::get<0>(b);
                });

                // Get the results
                blocks.clear();
                indices.Clear();

                for (auto& e : data) {
                    blocks.push_back(std::get<0>(e));
                    indices.Append(std::get<1>(e));
                }

                std::cout << ToString() << std::endl << std::endl;*/
            }

            std::string ToString() const {
                std::string result = "#<";

                result += name;
                for (auto& block : blocks) {
                    result += ":" + std::to_string(block.indices);

                    switch (block.symmetry) {
                        case CoefficientDefinition::SymmetryType ::SYMMETRIC:
                            break;
                        case CoefficientDefinition::SymmetryType ::ANTISYMMETRIC:
                            result += ",antisymmetric";
                            break;
                        case CoefficientDefinition::SymmetryType ::NONE:
                            result += ",none";
                            break;
                    }

                    result += ":" + std::to_string(block.derivatives);
                }

                if (exchangeSymmetries.size() > 0) result += ":";

                for (int i=0; i<exchangeSymmetries.size(); ++i) {
                    result += exchangeSymmetries[i] ? "yes" : "no";
                    if (i < exchangeSymmetries.size()-1) result += ",";
                }

                result += ">";

                return result;
            }

            bool operator<(const CoefficientDefinition& other) const {
                if (blocks.size() < other.blocks.size()) return true;
                else if (blocks.size() > other.blocks.size()) return false;

                for (int i=0; i<blocks.size(); ++i) {
                    if (blocks[i] < other.blocks[i]) return true;
                    else if (blocks[i] > other.blocks[i]) return false;
                }

                for (int i=0; i<exchangeSymmetries.size(); ++i) {
                    if (exchangeSymmetries[i] && !other.exchangeSymmetries[i]) return true;
                    if (!exchangeSymmetries[i] && other.exchangeSymmetries[i]) return false;
                }

                return name < other.name;
            }

            bool operator<=(const CoefficientDefinition& other) const {
                if (blocks.size() < other.blocks.size()) return true;
                else if (blocks.size() > other.blocks.size()) return false;

                for (int i=0; i<blocks.size(); ++i) {
                    if (blocks[i] < other.blocks[i]) return true;
                    else if (blocks[i] > other.blocks[i]) return false;
                }

                for (int i=0; i<exchangeSymmetries.size(); ++i) {
                    if (exchangeSymmetries[i] && !other.exchangeSymmetries[i]) return true;
                    if (!exchangeSymmetries[i] && other.exchangeSymmetries[i]) return false;
                }

                return name <= other.name;
            }

            bool operator==(const CoefficientDefinition& other) const {
                if (blocks.size() != other.blocks.size()) return false;

                for (int i=0; i<blocks.size(); ++i) {
                    if (blocks[i] != other.blocks[i]) return false;
                }

                for (int i=0; i<exchangeSymmetries.size(); ++i) {
                    if (exchangeSymmetries[i] != other.exchangeSymmetries[i]) return false;
                }

                return name == other.name;
            }

            inline bool operator>(const CoefficientDefinition& other) const {
               return other < *this;
            }

            inline bool operator>=(const CoefficientDefinition& other) const {
                return other <= *this;
            }

            inline bool operator!=(const CoefficientDefinition& other) const {
                return !(*this == other);
            }

            std::string name;
            std::vector<Block> blocks;
            std::vector<bool> exchangeSymmetries;
            Indices indices;
        };

        /**
            \class CoefficientParser

            Class to parse coefficients from input text and return the definition.
            The syntax is

                #<{namespace}:
         */
        class CoefficientParser {
        public:
            CoefficientParser() = default;
        public:
            void Reset() {
                inCoeff = false;
                isFinished = false;
                ignoreNextToken = false;
                buffer = "";
                this->current.clear();
            }

            void Parse(char current, char lookAhead = 0) {
                // If we are not inside of a coefficient
                if (!inCoeff) {
                    // If we do not see the beginning of a new coefficient, do nothing
                    if (current != '#' || lookAhead != '<') return;
                    else {
                        // Setup a new coefficient
                        inCoeff = true;
                        isFinished = false;
                        ignoreNextToken = true; // Need to take care of the fact that the definition of a coefficient starts with two chars
                        buffer = "";
                        this->current.clear();
                        wholeBuffer = "#<";
                        return;
                    }
                }

                // Ignore the token?
                if (ignoreNextToken) {
                    ignoreNextToken = false;
                    return;
                }

                wholeBuffer += std::string(1, current);

                // Deal with input
                if (current == '>') {
                    inCoeff = false;
                    isFinished = true;
                    this->current.push_back(buffer);

                    // Setup the definition of the coefficient
                    BuildCoefficientDefinition();
                } else if (current != ':') {
                    buffer += std::string(1, current);
                } else {
                    this->current.push_back(buffer);
                    buffer = "";
                }
            }

            void BuildCoefficientDefinition() {
                // Check if the input is valid
                if (current.size() == 0) {
                    throw std::runtime_error("Invalid coefficient definition. Cannot read coefficient `#<>`.");
                }

                coeff = CoefficientDefinition();

                // Get name
                coeff.name = PopFront();

                // Try to add blocks
                while (IsBlock()) {
                    auto vec = SplitByComma(PopFront());
                    unsigned int numDerivatives = std::stoi(PopFront());
                    unsigned int numIndices = std::stoi(vec[0]);
                    auto symmetry = CoefficientDefinition::SymmetryType::SYMMETRIC;

                    if (vec.size() > 1) {
                        auto def = vec[1];
                        if (def == "symmetric") symmetry = CoefficientDefinition::SymmetryType::SYMMETRIC;
                        else if (def == "antisymmetric") symmetry = CoefficientDefinition::SymmetryType::ANTISYMMETRIC;
                        else if (def == "none") symmetry = CoefficientDefinition::SymmetryType::NONE;
                    }

                    coeff.AddBlock(numIndices, numDerivatives, symmetry);
                }

                // If there are no indices
                if (!IsIndex()) throw std::runtime_error("Invalid coefficient definition. Need indices");

                // If we have an index
                coeff.indices = Indices::FromString(PopFront());

                unsigned total = 0;
                for (auto& block : coeff.blocks) {
                    total += block.indices + block.derivatives;
                }

                if (coeff.indices.Size() != total) {
                    if (total != 0) throw std::runtime_error("Invalid coefficient definition `" + wholeBuffer + "`. The number of indices does not match (Expected " + std::to_string(total) + ", got " + std::to_string(coeff.indices.Size()) + ").");

                    // Don't apply magic sauce
                    coeff.blocks.push_back(CoefficientDefinition::Block{ static_cast<unsigned>(coeff.indices.Size()), 0, CoefficientDefinition::SymmetryType::NONE });
                }

                // If we have an exchange symmetry entry
                if (IsExchangeSymmetry()) {
                    auto vec = SplitByComma(PopFront());

                    // If we have only one entry, treat all blocks equally
                    if (vec.size() == 1) {
                        while (vec.size() != coeff.blocks.size()-1) vec.push_back(vec[0]);
                    }

                    // If we do not have enough entries for the exchange symmetries, complain
                    if (vec.size() != coeff.blocks.size()-1) throw std::runtime_error("Invalid coefficient definition. Missing exchange symmetry information.");

                    for (auto& e : vec) {
                        coeff.exchangeSymmetries.push_back(e == "yes");
                    }
                }

                // If there was no exchange symmetry block treat it as yes
                if (coeff.exchangeSymmetries.size() == 0) {
                    while (coeff.exchangeSymmetries.size() != coeff.blocks.size() - 1) {
                        coeff.exchangeSymmetries.push_back(true);
                    }
                }

                // Expect no further input
                if (current.size() != 0) std::runtime_error("Invalid coefficient definition.");
            }
        private:
            std::string PopFront() {
                auto result = current[0];
                current.erase(current.begin());
                return result;
            }

            static std::vector<std::string> SplitByComma(const std::string& block) {
                std::vector<std::string> result;
                std::string buffer = "";

                for (auto& c : block) {
                    if (c == ',') {
                        // Translate the second block to lower case
                        result.push_back(buffer);
                        buffer = "";
                    } else {
                        buffer += std::string(1, tolower(c));
                    }
                }
                result.push_back(buffer);

                return result;
            }
        public:
            bool IsInCoefficient() const { return inCoeff; }
            bool IsFinished() const { return isFinished; }

            bool IsValidInput() const {
                return IsIndex() || IsExchangeSymmetry() || IsBlock();
            }

            bool IsIndex() const {
                if (current.size() < 1) return false;
                return current[0][0] == '{' && current[0][current[0].size()-1] == '}';
            }

            bool IsExchangeSymmetry() const {
                if (current.size() < 1) return false;

                auto vec = SplitByComma(current[0]);

                for (auto& e : vec) {
                    if (e != "yes" && e != "no") return false;
                }

                return true;
            }

            bool IsBlock() const {
                if (current.size() < 2) return false;

                auto vec = SplitByComma(current[0]);

                std::string numIndices = vec[0];
                std::string numDerivatives = current[1];

                try {
                    std::stoi(numIndices);
                    std::stoi(numDerivatives);
                } catch(...) {
                    return false;
                }

                if (vec.size() == 2)
                    return (vec[1] == "symmetric" || vec[1] == "antisymmetric" || vec[2] == "none");
                else if (vec.size() == 1) return true;
                else return false;
            }

            CoefficientDefinition GetDefinition() const {
                return coeff;
            }
        private:
            CoefficientDefinition coeff;
            std::string wholeBuffer;
            std::string buffer;
            std::vector<std::string> current;
            bool inCoeff = false;
            bool isFinished = false;
            bool ignoreNextToken = false;
        };

        /**
            \class Coefficient

            Container class that handles the calculation of a specific
            coefficient in a set of equations. It is calculated in the
            background once Start is called.

            Once the calculation is finished, the state changes and one
            can access the tensor via Get(). A call of Get() before the
            thread is finished will block the main thread, so be careful!
         */
        class Coefficient : public Unique<Coefficient, 103>, public std::enable_shared_from_this<Coefficient> {
        public:
            /**
                The states of a coefficient
             */
            enum State {
                DEFERRED = 1,
                CALCULATING = 2,
                FINISHED = 3,
                ABORTED = 4
            };
        public:
            Coefficient(CoefficientDefinition defn, const std::string& id) : defn(defn), id(id), state(DEFERRED) {
                name = id + GetRandomString(4);
            }

            virtual ~Coefficient() {
                // Join the thread
                thread.join();
            }
        public:
            // Is the coefficient calculation deferred, i.e. not started yet?
            bool IsDeferred() const { return state == DEFERRED; }

            // Is the coefficient currently calculating?
            bool IsCalculating() const { return state == CALCULATING; }

            // Is the coefficient calculation finished?
            bool IsFinished() const { return state == FINISHED; }

            // Is the coefficient calculation aborted due to an error?
            bool IsAborted() const { return state == ABORTED; }
        public:
            // Return a pointer to the coefficient
            std::shared_ptr<Coefficient> GetReference() {
                return shared_from_this();
            }
        public:
            typedef std::function<void(const std::shared_ptr<Coefficient>& coefficient)>   ObserverFunction;

            /**
                Register an observer that is notified once the
                state of the tensor changes
             */
            void RegisterObserver(ObserverFunction observer) {
                observers.push_back(observer);
            }
        private:
            /**
                This will notify all registered observers about
                the current state change.
             */
            void Notify() {
                // Get reference to the coefficient
                auto ref = GetReference();

                Construction::Logger logger;
                logger << Construction::Logger::DEBUG << "Notify all the observers of " << ref << Construction::Logger::endl;

                // Iterate over all observers
                for (auto& fn : observers) {
                    fn(ref);
                }
            }
        public:
            /**
                Start the calculation of the coefficient in a new thread
             */
            void Start() {
                // Start calculation in thread
                thread = std::thread(&Coefficient::Calculate, this);
            }

            /**
                Blocks the current thread until the calculation was either
                not started, is finished or an error occured.
             */
            void Wait() {
                std::unique_lock<std::mutex> lock(mutex);

                variable.wait(lock, [&]() {
                    return state != CALCULATING;
                });
            }
        public:
            std::string GetName() const { return name; }

            CoefficientDefinition GetDefinition() const { return defn; }
        public:
            /**
                Returns the tensor if it was calculated, otherwise it blocks
                until the tensor was calculated. If it was aborted due to
                an error, the result will be nullptr
             */
            std::shared_ptr<Tensor::Tensor> Get() {
                // Wait to be finished
                Wait();

                return (state == FINISHED) ? tensor : nullptr;
            }

            /**
                Returns the tensor if it was calculated, otherwise a nullptr
                without blocking the main thread
             */
            std::shared_ptr<Tensor::Tensor> GetAsync() {
                return (state == FINISHED) ? tensor : nullptr;
            }

            inline void SetTensor(Tensor::Tensor tensor) {
                this->tensor = std::make_shared<Tensor::Tensor>(tensor);
            }

            void Lock() {
                readMutex.lock();
            }

            void Unlock() {
                readMutex.unlock();
            }

            bool IsLocked() const {
                bool isUnlocked = readMutex.try_lock();
                if (isUnlocked) readMutex.unlock();
                return !isUnlocked;
            }
        public:
            /**
                \brief Calculate the number of steps needed to calculate the coefficient

                Calculates the actual number of steps that need to be calculated during
                generation of the coefficient.
             */
            unsigned int GetNumberOfSteps() const {
                unsigned int result = 2; // Arbitrary + Simplify

                // Add symmetrization of the blocks
                for (auto block : defn.blocks) {
                    // Symmetrization of block
                    if (block.indices > 1 && block.symmetry != CoefficientDefinition::SymmetryType::NONE) ++result;

                    // Symmetrization of derivative blocks
                    if (block.derivatives > 1) ++result;
                }

                // Exchange symmetries
                for (auto exchangeSymmetry : defn.exchangeSymmetries) {
                    if (exchangeSymmetry) ++result;
                }

                return result;
            }
        private:
            /**
                \brief Calculates the actual tensor with the correct symmetries

                Calculates the actual tensor with the correct symmetries. It
                shall not be used outside of the Start method, since this will
                generate the thread in the correct fashion and garantuees the
                deferred calculation to work.

                In between steps, the tensor will be stored on disk s.t. a
                new start of the program does not require us to calculate this
                coefficient again.
             */
            void Calculate() {
                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                try {
                    // Set the state to calculating
                    state = CALCULATING;

                    // Dealing with a constant
                    if (defn.indices.Size() == 0) {
                        tensor = std::make_shared<Construction::Tensor::Tensor>(
                                Construction::Tensor::Scalar(GetRandomString() + "_1") * Construction::Tensor::Tensor::One()
                        );

                        Notify(); // Arbitrary
                        Notify(); // Simplify
                    } else {
                        // ------ Arbitrary(...) --------

                        auto db = Construction::Tensor::ExpressionDatabase::Instance();

                        // Get index blocks
                        std::vector<std::pair<Indices, CoefficientDefinition::SymmetryType>> blocks;
                        std::vector<Indices> exchangedIndices;
                        Indices indices;

                        // Prepare all the index blocks and required symmetrization
                        {
                            unsigned offset = 0;
                            for (auto& block : defn.blocks) {
                                // DOF block
                                auto block_indices = Construction::Tensor::Indices::GetRomanSeries(block.indices,
                                                                                                   {1, 3}, offset);
                                blocks.push_back(std::make_pair(block_indices, block.symmetry));
                                offset += block.indices;
                                indices.Append(block_indices);

                                // derivative block
                                auto derivative_indices = Construction::Tensor::Indices::GetRomanSeries(
                                        block.derivatives, {1, 3}, offset);
                                blocks.push_back(std::make_pair(derivative_indices,
                                                                CoefficientDefinition::SymmetryType::SYMMETRIC));
                                offset += block.derivatives;
                                indices.Append(derivative_indices);
                            }
                        }

                        // Generate the list of all the exchanged indices
                        {
                            for (int i=0; i<defn.exchangeSymmetries.size(); ++i) {
                                if (!defn.exchangeSymmetries[i]) continue;

                                // Check if necessary
                                if (defn.blocks[i] != defn.blocks[i+1]) continue;

                                Indices inds;
                                for (int j=0; j<blocks.size()/2; ++j) {
                                    if (j != i) {
                                        inds.Append(blocks[2*j].first);
                                        inds.Append(blocks[2*j+1].first);
                                    } else {
                                        inds.Append(blocks[2*j+2].first);
                                        inds.Append(blocks[2*j+3].first);
                                        inds.Append(blocks[2*j].first);
                                        inds.Append(blocks[2*j+1].first);
                                        ++j;
                                    }
                                }
                                exchangedIndices.push_back(inds);
                            }
                        }

                        // ---------- Arbitrary -----------

                        // Generate current string
                        std::string currentCmd = "Arbitrary(" + indices.ToCommand() + ")";
                        std::cout << currentCmd << std::endl;

                        // Generate the tensors
                        if (!db->Contains(currentCmd)) {
                            tensor = std::make_shared<Construction::Tensor::Tensor>(Construction::Language::API::Arbitrary(indices));

                            // Insert into the database
                            db->Insert(currentCmd, *tensor);
                        } else {
                            Construction::Logger::Debug("Found coefficient in database");

                            auto expr = db->Get(currentCmd).As<Construction::Tensor::Tensor>();
                            Construction::Logger::Debug("Found ", expr);

                            // Copy from database
                            tensor = std::make_shared<Construction::Tensor::Tensor>(expr);
                        }

                        Notify();

                        // ---------- Symmetrizations -----------

                        for (auto& block : blocks) {
                            // If there are no indices in the block, do nothing
                            if (block.first.Size() == 0 || block.first.Size() == 1) continue;

                            switch (block.second) {
                                case CoefficientDefinition::SymmetryType::NONE:
                                    // Do nothing
                                    break;

                                case CoefficientDefinition::SymmetryType::SYMMETRIC:
                                    // Make symmetric
                                    if (block.first.Size() > 1) {
                                        currentCmd = "Symmetrize(" + currentCmd + ", " + block.first.ToCommand() + ")";
                                        std::cout << currentCmd << std::endl;

                                        if (!db->Contains(currentCmd)) {
                                            tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Symmetrize(block.first));
                                            db->Insert(currentCmd, *tensor);
                                        } else {
                                            tensor = std::make_shared<Construction::Tensor::Tensor>(db->Get(currentCmd).As<Construction::Tensor::Tensor>());
                                        }
                                    }
                                    Notify();
                                    break;

                                case CoefficientDefinition::SymmetryType::ANTISYMMETRIC:
                                    // Make antisymmetric
                                    if (block.first.Size() > 1) {
                                        currentCmd = "AntiSymmetrize(" + currentCmd + ", " + block.first.ToCommand() + ")";

                                        if (!db->Contains(currentCmd)) {
                                            tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->AntiSymmetrize(block.first));
                                            db->Insert(currentCmd, *tensor);
                                        } else {
                                            tensor = std::make_shared<Construction::Tensor::Tensor>(db->Get(currentCmd).As<Construction::Tensor::Tensor>());
                                        }
                                    }
                                    Notify();
                                    break;
                            }
                        }

                        // ---------- Exchange Symmetrizations -----------

                        for (auto& exchanged : exchangedIndices) {
                            currentCmd = "ExchangeSymmetrize(" + currentCmd + ", " + indices.ToCommand() + ", " + exchanged.ToCommand() +")";
                            std::cout << currentCmd << std::endl;

                            if (!db->Contains(currentCmd)) {
                                tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->ExchangeSymmetrize(indices, exchanged));

                                db->Insert(currentCmd, *tensor);
                            } else {
                                tensor = std::make_shared<Construction::Tensor::Tensor>(db->Get(currentCmd).As<Construction::Tensor::Tensor>());
                            }

                            Notify();
                        }

                        // ---------- Simplify -----------
                        currentCmd = "LinearIndependent(" + currentCmd + ")";
                        std::cout << currentCmd << std::endl;
                        if (!db->Contains(currentCmd)) {
                            tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Simplify().RedefineVariables(GetRandomString()));

                            db->Insert(currentCmd, *tensor);
                        } else {
                            tensor = std::make_shared<Construction::Tensor::Tensor>(db->Get(currentCmd).As<Construction::Tensor::Tensor>());
                        }

                        Notify();

                        // Assign the tensor to the session
                        Session::Instance()->Set(name, std::move(*tensor));
                    }
                } catch (...) {
                    // In case of exception, just set the calculation to aborted
                    state = ABORTED;

                    // Notify all observers
                    Notify();
                    variable.notify_all();

                    return;
                }

                // Finished
                state = FINISHED;

                Construction::Logger logger;
                logger << Construction::Logger::DEBUG << "Finished coefficient " << GetReference() << ": `" << ToString() << "`" << Construction::Logger::endl;

                Notify();
                variable.notify_all();
            }
        public:
            /**
                Generate a random string out of alphabeticals
             */
            static std::string GetRandomString(int size = 3) {
                static std::random_device device;
                static std::mt19937 engine (device());

                static std::vector<char> elements = {
                    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
                };

                std::uniform_int_distribution<int> dist(0, elements.size()-1);

                std::string output = "";

                for (int i=0; i<size; i++) {
                    output += elements[dist(engine)];
                }

                return output;
            }
        public:
            std::string ToString(bool includeResult=true) const {
                std::stringstream ss;

                ss << defn.ToString();

                if (state == FINISHED && includeResult) {
                    ss << " = " << tensor->ToString();
                }

                return ss.str();
            }
        private:
            mutable std::mutex mutex;
            mutable std::mutex readMutex;

            std::condition_variable variable;
            std::thread thread;

            //Session session;
            State state;

            std::vector<ObserverFunction> observers;

            std::string id;
            std::string name;
            CoefficientDefinition defn;

            std::shared_ptr<Tensor::Tensor> tensor;
        };

        typedef std::shared_ptr<Coefficient>   CoefficientReference;

        /**
            \class Coefficients

            \brief Singleton class that stores all coefficients

            Singleton class that stores all coefficients with a given index
            structure.
         */
        class Coefficients : public Singleton<Coefficients> {
        public:
            struct Definition {
                CoefficientDefinition defn;

                std::string id;

                bool operator==(const Definition& other) const {
                    return defn == other.defn && id == other.id;
                }

                bool operator!=(const Definition& other) const {
                    return !(*this == other);
                }

                bool operator<(const Definition& other) const {
                    if (defn > other.defn) return false;
                    else if (defn < other.defn) return true;
                    return id < other.id;
                }

                bool operator<=(const Definition& other) const {
                    if (defn > other.defn) return false;
                    else if (defn < other.defn) return true;
                    return id <= other.id;
                }

                inline bool operator>(const Definition& other) const {
                    return other < *this;
                }

                inline bool operator>=(const Definition& other) const {
                    return other <= *this;
                }
            };
        public:
            /*class DefinitionHasher {
            public:
                size_t operator()(const Definition& d) const {
                    auto result = std::hash<std::string>(d.id);

                    result = result ^ std::hash<std::string>(d.defn.name);

                    for (auto& block : d.defn.blocks) {
                        result = result ^ std::hash<unsigned>(block.indices);
                        result = result ^ std::hash<unsigned>(block.derivatives);
                        result = result ^ std::hash<int>(static_cast<int>(block.symmetry));
                    }

                    for (auto& exchange : d.defn.exchangeSymmetries) {
                        result = result ^ std::hash<bool>(exchange);
                    }

                    return result;
                }
            };*/
        public:
            CoefficientReference Get(CoefficientDefinition defn, const std::string& id) {
                Definition d;
                d.defn = defn;

                auto it = map.find(d);

                if (it != map.end()) {
                    return it->second;
                } else {
                    CoefficientReference ref = std::make_shared<Coefficient>(defn, id);
                    map.insert({ d, ref });
                    return ref;

                }
            }

            /**
                Start the calculation of all coefficients that were not
                started yet.
             */
            void StartAll() {
                for (auto& it : map) {
                    if (it.second->IsDeferred()) {
                        // Start
                        it.second->Start();
                    }
                }
            }

            size_t Size() const { return map.size(); }
        public:
            unsigned int GetNumberOfSteps() const {
                unsigned int steps = 0;
                for (auto& it : map) {
                    steps += it.second->GetNumberOfSteps();
                }
                return steps;
            }
        public:
            typedef std::map<Definition, CoefficientReference>      container_type;
            //typedef std::unordered_map<Definition, CoefficientReference, DefinitionHasher>   container_type;

            container_type::iterator begin() { return map.begin(); }
            container_type::iterator end() { return map.end(); }

            container_type::const_iterator begin() const { return map.begin(); }
            container_type::const_iterator end() const { return map.end(); }
        private:
            container_type map;
        };

        class CoefficientsLock {
        public:
            CoefficientsLock() {
                // Lock all the coefficients
                for (auto it = Coefficients::Instance()->begin(); it != Coefficients::Instance()->end(); ++it) {
                    if (it->second->IsFinished()) {
                        it->second->Lock();
                    }
                }
            }

            ~CoefficientsLock() {
                // Release all the coefficients
                for (auto it = Coefficients::Instance()->begin(); it != Coefficients::Instance()->end(); ++it) {
                    if (it->second->IsLocked() && it->second->IsFinished()) {
                        it->second->Unlock();
                    }
                }
            }
        };

    }
}
