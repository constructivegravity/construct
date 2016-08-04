#pragma once

#include <memory>
#include <vector>

#include <language/cli.hpp>
#include <equations/coefficient.hpp>

using Construction::Language::CLI;

namespace Construction {
    namespace Equations {

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
                SOLVED
            };
        public:
            // Constructor
            Equation(const std::string& code) : state(WAITING) {
                // Parse the code
                Parse(code);
            }

            ~Equation() {
                // Join the thread of the calculation
                thread.join();
            }
        public:
            bool IsWaiting() const { return state == WAITING; }
            bool IsSolving() const { return state == SOLVING; }
            bool IsSolved() const { return state == SOLVED; }
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
                unsigned l, ld, r, rd;
                std::string id;
                unsigned mode=0;

                // Recognize a coefficient
                for (int i=0; i<code.size(); ++i) {
                    char c = code[i];

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
                        }

                        temp = "";
                        mode++;
                        continue;
                    }

                    if (c == '>') {
                        inCoeff = false;
                        mode = 0;

                        // Get the coefficient reference
                        auto ref = Coefficients::Instance()->Get(l, ld, r, rd, id);

                        // Add the notification method
                        ref->RegisterObserver(std::bind(&Equation::OnCoefficientCalculated, this, std::placeholders::_1));

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

                        temp = "";

                        // Put on the list
                        coefficients.push_back(std::move(ref));
                        continue;
                    }

                    temp += c;
                }

                eq = "subst = HomogeneousSystem(" + current + "):";
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

                // Solve the equation in a new thread#
                this->thread = std::thread(&Equation::Solve, this);
            }

            void Solve() {
                std::unique_lock<std::mutex> lock(mutex);

                // Set the state to solving
                state = SOLVING;

                //   I. Lock the coefficients, so that no other equation can
                //      change it in the meantime
                // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                for (auto& coeff : coefficients) {
                    coeff->Lock();
                }

                //  II. Use the CLI to parse the equation and execute it
                //      to obtain the substitution
                // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                CLI cli;

                // Set the coefficient of the session
                cli(eq);

                // III. Convert the output into a substitution
                // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                auto subst = Session::Instance()->GetCurrent().As<Tensor::Substitution>();

                //  IV. Substitute the result into the coefficients
                // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                for (auto& coeff : coefficients) {
                    coeff->SetTensor(subst(*coeff->GetAsync()));

                    // Overwrite the tensor in the session
                    Session::Instance()->Get(coeff->GetName()) = *coeff->GetAsync();
                }

                //   V. Release locks
                // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                for (auto& coeff : coefficients) {
                    coeff->Unlock();
                }

                // Set the state to solved
                state = SOLVED;

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
            void Wait() {
                std::unique_lock<std::mutex> lock(mutex);

                variable.wait(lock, [&]() {
                    return state == SOLVED;
                });
            }
        private:
            std::thread thread;
            std::mutex mutex;
            std::condition_variable variable;

            std::string eq;
            std::vector<CoefficientReference> coefficients;

            std::vector<ObserverFunction> observers;

            State state;
        };

    }
}
