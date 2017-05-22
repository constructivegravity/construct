#pragma once

#include <memory>
#include <vector>

#include <common/singleton.hpp>
#include <language/cli.hpp>
#include <equations/coefficient.hpp>

using Construction::Language::CLI;

namespace Construction {
    namespace Equations {

        /**
            \class SubstitutionManager

            \brief Class that manages the substitution of results from equations
                   into the coefficients.

            Class that manages the substitution of results from equations
            Class that manages the substitution of results from equations
            into the coefficients. This works by a ticket system, i.e. any
            equation that can be calculated asks the manager for a ticket.
            If there are no tickets available for now, the equation thread
            is blocked until new tickets are available.

            If an equation has a ticket, it can be evaluated and the resulting
            substitution is send by the ticket back into the manager.
         */
        class SubstitutionManager : public Singleton<SubstitutionManager> {
        public:
            // The manager is either serving a locked
            enum State {
                SERVING,
                LOCKED
            };
        protected:
            /**
                \class Ticket


             */
            class Ticket : public std::enable_shared_from_this<Ticket> {
            public:
                enum State {
                    WAITING,
                    FULFILLED
                };
            public:
                void Fulfill(const Substitution& substitution) {
                    SubstitutionManager::Instance()->Fulfill(shared_from_this(), substitution);
                }
            public:
                friend class SubstitutionManager;
            private:
                State state;
            };

            void Fulfill(std::shared_ptr<Ticket> ticket, const Substitution& substition) {
                {
                    // Lock the mutex
                    std::unique_lock<std::mutex> lock(mutex);

                    // Check if the ticket was already served
                    auto it = std::find(tickets.begin(), tickets.end(), ticket);
                    if (it == tickets.end()) return;

                    // Remove the ticket from the list and mark it fulfilled
                    tickets.erase(it);
                    ticket->state = Ticket::FULFILLED;

                    // Insert the substitution
                    substitutions.push_back(substition);

                    // Set the state to locked to keep the manager
                    // from giving further tickets
                    state = LOCKED;

                    Construction::Logger::Debug("Fulfilled ticket ", ticket);
                }

                // If no ticket left, apply the substitutions
                if (tickets.size() == 0) {
                    Apply();
                }
            }
        public:
            std::shared_ptr<Ticket> GetTicket() {
                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                // Wait for the thread to be ticket less
                variable.wait(lock, [&]() {
                    return state == SERVING;
                });

                // Create ticket
                auto ticket = std::make_shared<Ticket>();

                // Add to the list and return one for the thread
                tickets.push_back(ticket);

                // If the number of tickets is equal to a threshold
                // set the state to LOCKED
                if (tickets.size() == maxTickets) {
                    state = LOCKED;
                }

                Construction::Logger logger;
                logger << Construction::Logger::DEBUG << "Issued ticket " << ticket << Construction::Logger::endl;

                return std::move(ticket);
            }
        public:
            void SetMaxTickets(int tickets=4) {
                maxTickets = tickets;
            }
        private:
            void Apply() {
                // Lock the mutex
                std::unique_lock<std::mutex> lock(mutex);

                Construction::Logger::Debug("Apply substitutions (from ", substitutions.size(), " tickets)");

                // Merge
                auto merged = Tensor::Substitution::Merge(substitutions);

                Construction::Logger::Debug("Merged substitutions into ", merged);

                // Reset the list of substitutions
                substitutions.clear();

                // Lock all the coefficients
                CoefficientsLock coeffsLock;

                // Iterate over all coefficients and apply the
                for (auto& pair : *Coefficients::Instance()) {
                    auto ref = pair.second;

                    if (ref->IsFinished()) {
                        ref->SetTensor(merged(*ref->GetAsync()).FastSimplify());

                        Construction::Logger::Debug("Updated coefficient: ", ref->ToString());

                        // Overwrite the tensor in the session
                        Session::Instance()->Set(ref->GetName(), *ref->GetAsync());
                    }
                }

                // Set the state back to serving
                state = SERVING;

                // Wake up all sleeping threads
                variable.notify_all();
            }
        private:
            State state;

            int maxTickets = 4;

            std::vector<Tensor::Substitution> substitutions;
            std::vector<std::shared_ptr<Ticket>> tickets;

            std::mutex mutex;
            std::condition_variable variable;
        };

        /**
            \class Equation

            Representing a single equation in a system of equations.
            It is constructed with a command string, which is a superset of
            the construct language, with the new feature that one can specify
            unique coefficients via

                #<id:l:ld:r:rd:{indices}>

            where id is the identifier of the coefficient (usually just a number),
            l and r denote the number of indices in the left/right block, and
            ld and rd denote the number of indices in the left/right derivative blocks.

            This allows one to easily specify equations between different
            coefficients, e.g.

                Add(Symmetrize(#<lambda:2,0,2,0,{a b c d}>, {b d}, #<mu:4,0,0,0,{a b c d}>)

            which corresponds to a equation of the form (in LaTeX code)

                0 = \lambda_{a(b|c|d)} + \mu_{a b c d}

            with the right symmetries for lambda.

            Once all the coefficients in the equation are calculated, the
            equation is solved in a separate thread.
         */
        class Equation {
        public:
            enum State {
                WAITING,
                SOLVING,
                SOLVED,
                ABORTED
            };
        public:
            // Constructor
            Equation(const std::string& code) : state(WAITING), code(code) {
                // Parse the code
                Parse(code);
            }

            ~Equation() {
                // Join the thread of the calculation
                if (!isEmpty) {
                    thread.join();
                }
            }
        public:
            bool IsWaiting() const { return state == WAITING; }
            bool IsSolving() const { return state == SOLVING; }
            bool IsSolved() const { return state == SOLVED; }

            bool IsEmpty() const { return isEmpty; }
        public:
            std::string GetCode() const { return code; }
        public:
            /**
                \brief Parses the expression

                Parses the expression. All occuring coefficients are
                extracted and the equation is registered as an observer for
                the coefficients.

                Everything that is not a coefficient will be put in the
                equation string. Note that there is no syntax checking at this
                stage.
             */
            void Parse(const std::string& code) {
                bool inCoeff = false;
                int coeffStart = -1;
                std::string current;
                std::string temp;
                std::string tmp2;
                unsigned l, ld, r, rd;
                bool foundOptional=false;
                bool exchangeSymmetry = true;
                std::string id;
                unsigned mode=0;

                // Recognize a coefficient
                for (int i=0; i<code.size(); ++i) {
                    char c = code[i];

                    // Ignore comments
                    if (c == '/' && i<code.size()-1 && code[i+1] == '/' ) {
                        break;
                    }

                    if (!inCoeff) {
                        if (c == '#' && i < code.size()-1 & code[i+1] == '<') {
                            inCoeff = true;
                            coeffStart = i;
                            i++;
                            continue;
                        }

                        current += c;
                        continue;
                    }

                    if (c == ':') {
                        if (mode == 0) {
                            id = temp;
                        } else if (mode == 1) {
                            l = std::stoi(temp);
                        } else if (mode == 2) {
                            ld = std::stoi(temp);
                        } else if (mode == 3) {
                            r = std::stoi(temp);
                        } else if (mode == 4) {
                            rd = std::stoi(temp);
                        } else if (mode == 5) {
                            foundOptional = true;
                            tmp2 = temp;
                        }

                        temp = "";
                        mode++;
                        continue;
                    }

                    if (c == '>') {
                        inCoeff = false;
                        mode = 0;

                        if (foundOptional) {
                            if (temp == "no") {
                                exchangeSymmetry = false;
                            }

                            temp = tmp2;
                        }

                        // Bring the coefficients into canonical order.
                        // Since we have the exchange symmetry, this is of course
                        // always possible.
                        if (r < l || (r == l && rd < ld)) {
                            // Swap the blocks
                            auto tmp = l;
                            l = r;
                            r = tmp;

                            tmp = ld;
                            ld = rd;
                            rd = tmp;

                            auto indices = Indices::FromString(temp);

                            auto block1 = indices.Partial({0, r+rd-1});
                            auto block2 = indices.Partial({r+rd, r+rd+l+ld-1});

                            block2.Append(block1);

                            temp = block2.ToCommand();
                        }

                        // Get the coefficient reference
                        auto ref = Coefficients::Instance()->Get(l, ld, r, rd, id, exchangeSymmetry);

                        tmp2 = "";
                        exchangeSymmetry = true;
                        foundOptional = false;

                        // Replace the coefficient with a dummy name
                        {
                            std::stringstream ss;
                            ss << "RenameIndices(" << ref->GetName() << ", " ;

                            auto indices = Construction::Tensor::Indices::GetRomanSeries(l+ld+r+rd, {1,3});
                            ss << "{";
                            for (int i=0; i<indices.Size(); ++i) {
                                ss << indices[i];
                                if (i < indices.Size()-1) ss << " ";
                            }
                            ss << "}, ";
                            ss << temp << ")";

                            current += ss.str();
                        }

                        bool found=false;
                        for (auto& ref_ : coefficients) {
                            if (ref_ == ref) {
                                found = true;
                                break;
                            }
                        }

                        if (!found) {
                            // Add the notification method
                            ref->RegisterObserver(std::bind(&Equation::OnCoefficientCalculated, this, std::placeholders::_1));

                            // Put on the list
                            coefficients.push_back(std::move(ref));
                        }

                        temp = "";
                        continue;
                    }

                    temp += c;
                }

                // Check if the equation is trivial
                {
                    isEmpty = true;
                    for (auto& c : current) {
                        if (c != ' ') {
                            isEmpty = false;
                            break;
                        }
                    }
                }

                substName = "subst" + Coefficient::GetRandomString(3);
                testName = "test" + Coefficient::GetRandomString(3);

                eq = substName + " = HomogeneousSystem(" + current + "):";
                test = testName + " = " + current + ":";
            }
        public:
            /**
                \brief Callback that is invoked by finished coefficients

                Callback that is called by finished coefficients
             */
            void OnCoefficientCalculated(const CoefficientReference& coefficient) {
                // Check if all coefficients are calculated
                for (auto& c : coefficients) {
                    // If not finished, do nothing
                    if (!c->IsFinished()) {
                        return;
                    }
                }

                // Lock the mutex
                std::unique_lock<std::mutex> lock(startMutex);

                Construction::Logger logger;
                logger << Construction::Logger::DEBUG << "Finished all coefficients for equation `" << eq << "`" << Construction::Logger::endl;

                if (state == WAITING) {
                    // Solve the equation in a new thread#
                    this->thread = std::thread(&Equation::Solve, this);
                }
            }

            void Solve() {
                std::unique_lock<std::mutex> lock(mutex);

                // Set the state to solving
                state = SOLVING;

                Construction::Logger logger;
                logger << Construction::Logger::DEBUG << "Start solving equation `" << eq << "`" << Construction::Logger::endl;

                //   I. Get a ticket to promise to yield a substitution
                //      or wait until a ticket can be returned
                auto ticket = SubstitutionManager::Instance()->GetTicket();

                //  II. Use the CLI to parse the equation and execute it
                //      to obtain the substitution
                // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                CLI cli;

                // Set the coefficient of the session
                //try {
                    cli(eq);

                    // III. Convert the output into a substitution
                    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                    auto subst = Session::Instance()->Get(substName).As<Tensor::Substitution>();

                    // Store the substitution in the
                    this->substitution = subst;

                    Construction::Logger::Debug("Found substitution ", subst, " from equation ", eq);

                    //  IV. Give the substitution to the ticket
                    // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                    ticket->Fulfill(subst);
                /*} catch (const Exception& e) {
                    state = ABORTED;

                    // TODO: THROW EXCEPTION
                    Construction::Logger::Error("Error in equation `", eq, "`: ", e.what());

                    throw;

                    variable.notify_all();
                    Notify();

                    return;
                }*/

                // Set the state to solved
                state = SOLVED;

                logger << Construction::Logger::DEBUG << "Solved equation `" << eq << "`" << Construction::Logger::endl;

                variable.notify_all();
                Notify();
            }
        public:
            typedef std::function<void(const Equation& eq)>     ObserverFunction;

            void RegisterObserver(ObserverFunction observer) {
                observers.push_back(observer);
            }

            void Notify() const {
                for (auto& observer : observers) {
                    observer(*this);
                }
            }
        public:
            bool Test(Tensor::Tensor* output=nullptr) {
                // Wait to be finished
                Wait();

                // Execute the test
                CLI cli;
                cli(test);

                // Get the result
                auto testResult = Session::Instance()->Get(testName).As<Tensor::Tensor>().CollectByVariables();

                // Improve the expression
                {
                    auto summands = testResult.GetSummands();
                    std::vector<Tensor::Tensor> simplified;

                    for (auto& tensor : summands) {
                        auto pair = tensor.SeparateScalefactor();
                        auto s = pair.second.Simplify();

                        if (!s.IsZeroTensor()) {
                            simplified.push_back(pair.first * s);
                        }
                    }

                    testResult = Tensor::Tensor::Add(simplified);
                }

                // Set the output
                if (output) {
                    *output = testResult;
                }

                // Check if the tensor is zero
                return testResult.IsZeroTensor();
            }
        public:
            Tensor::Substitution GetSubstition() {
                if (state != SOLVED) Wait();
                return substitution;
            }
        public:
            void Wait() {
                std::unique_lock<std::mutex> lock(mutex);

                variable.wait(lock, [&]() {
                    return state == SOLVED;
                });
            }

            std::string ToLaTeX() const {
                CLI cli;
                std::string output = cli.ToLaTeX(eq);

                // Crawl for all indices
                std::vector<std::pair<std::string, std::string>> crawledCoefficients;
                {
                    size_t pos = 0;
                    while (true) {
                        pos = eq.find("RenameIndices(", pos);
                        if (pos == std::string::npos) break;
                        pos += 14;

                        std::string name, indices;

                        while (eq[pos] != ',') {
                            name += std::string(1, eq[pos]);
                            ++pos;
                        }

                        // Ignore the first indices block { }
                        pos = eq.find("}, ", pos);
                        pos += 3;

                        // Get the indices
                        while (eq[pos] != ')') {
                            indices += std::string(1, eq[pos]);
                            ++pos;
                        }

                        crawledCoefficients.push_back({ name, indices });
                    }
                }

                // Replace all the coefficients
                size_t pos = 0;
                for (auto& pair : crawledCoefficients) {
                    // Jump to the position in the output
                    pos = output.find(pair.first, pos);
                    if (pos == std::string::npos) return "Error building LaTeX code";

                    // Find the coefficient
                    CoefficientReference ref;
                    for (auto& coef : coefficients) {
                        if (coef->GetName() == pair.first) {
                            ref = coef;
                            break;
                        }
                    }

                    auto text = ref->ToString(false) + "_" + pair.second;

                    // Replace in the output
                    output.erase(pos, pair.first.size());
                    output.insert(pos, text);

                    pos += text.size();
                }

                // Replace the substitution by a zero
                output.erase(0, substName.size());
                output.insert(0, "0");

                return output;
            }
        private:
            std::thread thread;
            std::mutex mutex;
            std::mutex startMutex;
            std::condition_variable variable;

            bool isEmpty;

            std::string code;
            std::string eq;
            std::string test;
            std::string substName;
            std::string testName;
            std::vector<CoefficientReference> coefficients;

            Tensor::Substitution substitution;

            std::vector<ObserverFunction> observers;

            State state;
        };

    }
}
