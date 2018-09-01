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
                using sort_t = std::tuple<Block, Indices, int>;
                std::vector<sort_t> data;
                Indices old = indices;

                for (int i=0; i<blocks.size(); ++i) {
                    auto ind = old.Partial({ 0, blocks[i].indices + blocks[i].derivatives-1 });
                    old = old.Partial({ blocks[i].indices + blocks[i].derivatives, static_cast<unsigned int>(old.Size())-1 });

                    data.push_back(std::make_tuple(blocks[i], ind, i));
                }

                // Sort
                std::sort(data.begin(), data.end(), [&](const sort_t& a, const sort_t& b) {
                    if (!this->exchangeSymmetries[std::get<2>(a)]) return true;
                    return std::get<0>(a) < std::get<0>(b);
                });

                // Get the results
                blocks.clear();
                indices.Clear();

                for (auto& e : data) {
                    blocks.push_back(std::get<0>(e));
                    indices.Append(std::get<1>(e));
                }
            }

            std::string name;
            std::vector<Block> blocks;
            std::vector<bool> exchangeSymmetries;
            Indices indices;
            bool appyMagicSauce=true;
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
                if (!IsIndex() && coeff.blocks.size() > 0) throw std::runtime_error("Invalid coefficient definition. Need indices");

                // If we have an index
                if (IsIndex()) {
                    coeff.indices = Indices::FromString(PopFront());

                    unsigned total = 0;
                    for (auto& block : coeff.blocks) {
                        total += block.indices + block.derivatives;
                    }

                    if (coeff.indices.Size() != total) throw std::runtime_error("Invalid coefficient definition. The number of indices does not match.");

                    // Apply magic sauce?
                    if (coeff.blocks.size() == 0) coeff.appyMagicSauce = false;
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
            Coefficient(unsigned indices, const std::string& id)
                : applyMagicSauce(false), l(indices), ld(0), r(0), rd(0), id(id), exchangeSymmetry(false), state(DEFERRED)
            {
                name = id + GetRandomString(4);
            }

            // Constructor
            Coefficient(unsigned l, unsigned ld, unsigned r, unsigned rd, const std::string& id, bool exchangeSymmetry=true)
                : applyMagicSauce(true), l(l), ld(ld), r(r), rd(rd), id(id), exchangeSymmetry(exchangeSymmetry), state(DEFERRED)
            {
                // Generate random name
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

            unsigned GetNumberOfLeftIndices() const { return l; }
            unsigned GetNumberOfLeftDerivativeIndices() const { return ld; }
            unsigned GetNumberOfRightIndices() const { return r; }
            unsigned GetNumberOfRightDerivativeIndices() const { return rd; }
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

                    // If no indices, return a variable
                    if (applyMagicSauce) {
                        if (l == 0 && ld == 0 && r == 0 && rd == 0) {
                            tensor = std::make_shared<Construction::Tensor::Tensor>(
                                Construction::Tensor::Scalar(GetRandomString() + "_1") * Construction::Tensor::Tensor::One()
                            );

                            Notify(); // Arbitrary
                            Notify(); // Symmetrize 1
                            Notify(); // Symmetrize 2
                            Notify(); // Symmetrize 3
                            Notify(); // Symmetrize 4
                            Notify(); // Exchange Symmetrize
                            Notify(); // Simplify
                        } else {
                            auto db = Construction::Tensor::ExpressionDatabase::Instance();

                            // Get index blocks
                            auto block1 = Construction::Tensor::Indices::GetRomanSeries(l, {1,3});
                            auto block2 = Construction::Tensor::Indices::GetRomanSeries(ld, {1,3}, l);
                            auto block3 = Construction::Tensor::Indices::GetRomanSeries(r, {1,3}, l+ld);
                            auto block4 = Construction::Tensor::Indices::GetRomanSeries(rd, {1,3}, l+ld+r);

                            // Generate all the indices
                            auto indices = block1;
                            indices.Append(block2);
                            indices.Append(block3);
                            indices.Append(block4);

                            // Generate current string
                            std::string currentCmd = "Arbitrary(" + indices.ToCommand() + ")";

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

                            // Update the session of the coefficient
                            //session.SetCurrent(currentCmd, *tensor);

                            // Symmetrize first block if necessary
                            if (block1.Size() > 1) {
                                currentCmd = "Symmetrize(" + currentCmd + ", " + block1.ToCommand() + ")";

                                if (!db->Contains(currentCmd)) {
                                    tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Symmetrize(block1));
                                    db->Insert(currentCmd, *tensor);
                                } else {
                                    tensor = std::make_shared<Construction::Tensor::Tensor>(db->Get(currentCmd).As<Construction::Tensor::Tensor>());
                                }

                                //session.SetCurrent(currentCmd, *tensor);
                            }

                            Notify();

                            // Symmetrize second block if necessary
                            if (block2.Size() > 1) {
                                currentCmd = "Symmetrize(" + currentCmd + ", " + block2.ToCommand() + ")";

                                if (!db->Contains(currentCmd)) {
                                    tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Symmetrize(block2));
                                    db->Insert(currentCmd, *tensor);
                                } else {
                                    tensor = std::make_shared<Construction::Tensor::Tensor>(db->Get(currentCmd).As<Construction::Tensor::Tensor>());
                                }
                            }

                            Notify();

                            // Symmetrize first block if necessary
                            if (block3.Size() > 1) {
                                currentCmd = "Symmetrize(" + currentCmd + ", " + block3.ToCommand() + ")";

                                if (!db->Contains(currentCmd)) {
                                    tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Symmetrize(block3));
                                    db->Insert(currentCmd, *tensor);
                                } else {
                                    tensor = std::make_shared<Construction::Tensor::Tensor>(db->Get(currentCmd).As<Construction::Tensor::Tensor>());
                                }
                                //session.SetCurrent(currentCmd, *tensor);
                            }

                            Notify();

                            // Symmetrize first block if necessary
                            if (block4.Size() > 1) {
                                currentCmd = "Symmetrize(" + currentCmd + ", " + block4.ToCommand() + ")";

                                if (!db->Contains(currentCmd)) {
                                    tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Symmetrize(block4));
                                    db->Insert(currentCmd, *tensor);
                                } else {
                                    tensor = std::make_shared<Construction::Tensor::Tensor>(db->Get(currentCmd).As<Construction::Tensor::Tensor>());
                                }

                                //session.SetCurrent(currentCmd, *tensor);
                            }

                            Notify();

                            // Do the block exchange if necessary
                            if (l == r && ld == rd && exchangeSymmetry) {
                                auto exchanged = block3;
                                exchanged.Append(block4);
                                exchanged.Append(block1);
                                exchanged.Append(block2);

                                currentCmd = "ExchangeSymmetrize(" + currentCmd + ", " + indices.ToCommand() + ", " + exchanged.ToCommand() +")";

                                if (!db->Contains(currentCmd)) {
                                    tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->ExchangeSymmetrize(indices, exchanged));

                                    db->Insert(currentCmd, *tensor);
                                } else {
                                    tensor = std::make_shared<Construction::Tensor::Tensor>(db->Get(currentCmd).As<Construction::Tensor::Tensor>());
                                }
                            }

                            Notify();
                            //session.SetCurrent(currentCmd, *tensor);

                            // Simplify and redefine variables
                            currentCmd = "LinearIndependent(" + currentCmd + ")";
                            if (!db->Contains(currentCmd)) {
                                tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Simplify().RedefineVariables(GetRandomString()));

                                db->Insert(currentCmd, *tensor);
                            } else {
                                tensor = std::make_shared<Construction::Tensor::Tensor>(db->Get(currentCmd).As<Construction::Tensor::Tensor>());
                            }

                            Notify();
                        }

                        Session::Instance()->Set(name, std::move(*tensor));
                    } else {
                        auto db = Construction::Tensor::ExpressionDatabase::Instance();

                        // Get index blocks
                        auto indices = Construction::Tensor::Indices::GetRomanSeries(l, {1,3});

                        // Generate current string
                        std::string currentCmd = "Arbitrary(" + indices.ToCommand() + ")";

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

                        Notify(); // Arbitrary
                        Notify(); // Symmetrize 1
                        Notify(); // Symmetrize 2
                        Notify(); // Symmetrize 3
                        Notify(); // Symmetrize 4
                        Notify(); // Exchange Symmetrize

                        // Simplify and redefine variables
                        currentCmd = "LinearIndependent(" + currentCmd + ")";
                        if (!db->Contains(currentCmd)) {
                            tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Simplify().RedefineVariables(GetRandomString()));

                            db->Insert(currentCmd, *tensor);
                        } else {
                            tensor = std::make_shared<Construction::Tensor::Tensor>(db->Get(currentCmd).As<Construction::Tensor::Tensor>());
                        }

                        Notify(); // Simplify

                        Session::Instance()->Set(name, std::move(*tensor));
                    }

                    //session.SetCurrent(currentCmd, *tensor);
                } catch(...) {
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
                ss << "#<" << id << ":" << l << ":" << ld << ":" << r << ":" << rd;
                if (!exchangeSymmetry) ss << ":no";
                ss << ">";

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

            bool applyMagicSauce;
            unsigned l, ld, r, rd;
            std::string id;
            std::string name;
            bool exchangeSymmetry;

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
                unsigned l;
                unsigned ld;
                unsigned r;
                unsigned rd;
                std::string id;
                bool exchangeSymmetry;
                bool applyMagicSauce;

                bool operator==(const Definition& other) const {
                    return l == other.l && r == other.r && ld == other.ld && rd == other.rd && id == other.id && exchangeSymmetry == other.exchangeSymmetry && applyMagicSauce == other.applyMagicSauce;
                }

                bool operator!=(const Definition& other) const {
                    return !(*this == other);
                }

                bool operator<(const Definition& other) const {
                    if (l > other.l) return false;
                    else if (l < other.l) return true;

                    if (ld > other.ld) return false;
                    else if (ld < other.ld) return true;

                    if (r > other.r) return false;
                    else if (r < other.r) return true;

                    if (rd > other.rd) return false;
                    else if (rd < other.rd) return true;

                    if (!exchangeSymmetry && other.exchangeSymmetry) return false;
                    else if (exchangeSymmetry && !other.exchangeSymmetry) return true;

                    if (!applyMagicSauce && other.applyMagicSauce) return false;
                    else if (applyMagicSauce && !other.applyMagicSauce) return true;

                    return id < other.id;
                }

                bool operator<=(const Definition& other) const {
                    if (l > other.l) return false;
                    else if (l < other.l) return true;

                    if (ld > other.ld) return false;
                    else if (ld < other.ld) return true;

                    if (r > other.r) return false;
                    else if (r < other.r) return true;

                    if (rd > other.rd) return false;
                    else if (rd < other.rd) return true;

                    if (!exchangeSymmetry && other.exchangeSymmetry) return false;
                    else if (exchangeSymmetry && !other.exchangeSymmetry) return true;

                    if (!applyMagicSauce && other.applyMagicSauce) return false;
                    else if (applyMagicSauce && !other.applyMagicSauce) return true;

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
            class DefinitionHasher {
            public:
                size_t operator()(const Definition& d) const {
                    return std::hash<unsigned>()(d.l) ^ std::hash<unsigned>()(d.ld) ^ std::hash<unsigned>()(d.r) ^ std::hash<unsigned>()(d.rd) ^ std::hash<std::string>()(d.id) ^ std::hash<bool>()(d.exchangeSymmetry);
                }
            };
        public:
            CoefficientReference Get(unsigned indices, const std::string& id) {
                Definition d;
                d.l = indices; d.ld = 0; d.r = 0; d.rd = 0; d.id = id; d.exchangeSymmetry = false; d.applyMagicSauce = false;

                auto it = map.find(d);

                if (it != map.end()) {
                    return it->second;
                } else {
                    CoefficientReference ref = std::make_shared<Coefficient>(indices, id);
                    map.insert({ d, ref });
                    return ref;
                }
            }

            CoefficientReference Get(unsigned l, unsigned ld, unsigned r, unsigned rd, const std::string& id, bool exchangeSymmetry=true) {
                Definition d;
                d.l = l; d.ld = ld; d.r = r; d.rd = rd; d.id = id; d.exchangeSymmetry = exchangeSymmetry; d.applyMagicSauce = true;

                auto it = map.find(d);

                if (it != map.end()) {
                    return it->second;
                } else {
                    CoefficientReference ref = std::make_shared<Coefficient>(l, ld, r, rd, id, exchangeSymmetry);
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
