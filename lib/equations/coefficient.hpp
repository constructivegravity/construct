#pragma once

#include <unordered_map>
#include <mutex>
#include <thread>
#include <functional>
#include <random>
#include <memory>

#include <common/uuid.hpp>
#include <language/session.hpp>
#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <language/api.hpp>

using Construction::Common::Unique;
using Construction::Language::Session;
using Construction::Tensor::Tensor;
using Construction::Tensor::Indices;

namespace Construction {
    namespace Equations {

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
            // Constructor
            Coefficient(unsigned l, unsigned ld, unsigned r, unsigned rd, const std::string& id)
                : l(l), ld(ld), r(r), rd(rd), id(id), state(DEFERRED)
            {
                // Generate random name
                name = id + GetRandomString(4);
            }

            virtual ~Coefficient() {
                // Join the thread
                //thread.join();
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
                    std::string currentCmd = "Arbitrary({" + indices.ToString().substr(1) + ")";

                    // Generate the tensors
                    tensor = std::make_shared<Construction::Tensor::Tensor>(Construction::Language::API::Arbitrary(indices));

                    // Update the session of the coefficient
                    //session.SetCurrent(currentCmd, *tensor);

                    // Symmetrize first block if necessary
                    if (block1.Size() > 1) {
                        currentCmd = "Symmetrize(%, " + block1.ToString().substr(1) + "})";
                        tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Symmetrize(block1));

                        //session.SetCurrent(currentCmd, *tensor);
                    }

                    // Symmetrize second block if necessary
                    if (block2.Size() > 1) {
                        currentCmd = "Symmetrize(%, " + block2.ToString().substr(1) + "})";
                        tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Symmetrize(block2));

                        //session.SetCurrent(currentCmd, *tensor);
                    }

                    // Symmetrize first block if necessary
                    if (block3.Size() > 1) {
                        currentCmd = "Symmetrize(%, " + block3.ToString().substr(1) + "})";
                        tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Symmetrize(block3));

                        //session.SetCurrent(currentCmd, *tensor);
                    }

                    // Symmetrize first block if necessary
                    if (block4.Size() > 1) {
                        currentCmd = "Symmetrize(%, " + block4.ToString().substr(1) + "})";
                        tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Symmetrize(block4));

                        //session.SetCurrent(currentCmd, *tensor);
                    }

                    // Do the block exchange
                    auto exchanged = block3;
                    exchanged.Append(block4);
                    exchanged.Append(block1);
                    exchanged.Append(block2);

                    currentCmd = "ExchangeSymmetrize(%, " + indices.ToString().substr(1) + "}, " + exchanged.ToString().substr(1) +"})";
                    tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->ExchangeSymmetrize(indices, exchanged));

                    //session.SetCurrent(currentCmd, *tensor);

                    // Simplify and redefine variables
                    currentCmd = "LinearIndependent(%)";
                    tensor = std::make_shared<Construction::Tensor::Tensor>(tensor->Simplify().RedefineVariables(GetRandomString()));

                    Session::Instance()->Get(name) = *tensor;
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
            std::string ToString() const {
                std::stringstream ss;
                ss << "#<" << id << ":" << l << ":" << ld << ":" << ":" << r << ":" << rd << ">";

                if (state == FINISHED) {
                    ss << " = " << tensor->ToString();
                }

                return ss.str();
            }
        private:
            std::mutex mutex;
            std::mutex readMutex;

            std::condition_variable variable;
            std::thread thread;

            //Session session;
            State state;

            std::vector<ObserverFunction> observers;

            unsigned l, ld, r, rd;
            std::string id;
            std::string name;

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

                bool operator==(const Definition& other) const {
                    return l == other.l && r == other.r && ld == other.ld && rd == other.rd && id == other.id;
                }
            };
        public:
            class DefinitionHasher {
            public:
                size_t operator()(const Definition& d) const {
                    return std::hash<unsigned>()(d.l) ^ std::hash<unsigned>()(d.ld) ^ std::hash<unsigned>()(d.r) ^ std::hash<unsigned>()(d.rd) ^ std::hash<std::string>()(d.id);
                }
            };
        public:
            CoefficientReference Get(unsigned l, unsigned ld, unsigned r, unsigned rd, const std::string& id) {
                Definition d;
                d.l = l; d.ld = ld; d.r = r; d.rd = rd; d.id = id;

                auto it = map.find(d);

                if (it != map.end()) {
                    return it->second;
                } else {
                    CoefficientReference ref = std::make_shared<Coefficient>(l, ld, r, rd, id);
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
            std::unordered_map<Definition, CoefficientReference, DefinitionHasher>::iterator begin() { return map.begin(); }
            std::unordered_map<Definition, CoefficientReference, DefinitionHasher>::iterator end() { return map.end(); }

            std::unordered_map<Definition, CoefficientReference, DefinitionHasher>::const_iterator begin() const { return map.begin(); }
            std::unordered_map<Definition, CoefficientReference, DefinitionHasher>::const_iterator end() const { return map.end(); }
        private:
            std::unordered_map<Definition, CoefficientReference, DefinitionHasher> map;
        };

    }
}
