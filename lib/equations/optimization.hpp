#pragma once

#include <vector>
#include <random>
#include <iterator>

#include <tensor/scalar.hpp>
#include <tensor/substitution.hpp>

namespace Construction {
    namespace Equations {

        template<typename RandomGenerator=std::default_random_engine>
        class random_selector {
        public:
            random_selector(RandomGenerator g = RandomGenerator(std::random_device()())) : gen(g) {}

            template<typename Iter>
            Iter select(Iter start, Iter end) {
                std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
                std::advance(start, dis(gen));
                return start;
            }

            template<typename Iter>
            Iter operator()(Iter start, Iter end) {
                return select(start, end);
            }
        private:
            RandomGenerator gen;
        };

        using Construction::Tensor::Scalar;
        using Construction::Tensor::Substitution;

        struct Species {
            std::vector<Scalar> expressions;
            Substitution substitution;
        };

        class Optimizer {
        public:
            Optimizer(const std::vector<Scalar>& expressions, unsigned numElements = 1000) : numElements(numElements) {
                Species s;
                s.expressions = expressions;
                elements.push_back(s);
            }
        public:
            double CostFunction(const Species& s) const {
                return 0;
            }
        public:
            void ScalarMutation(Species& species, Substitution& substitution) {

            }

            void AdditiveMutation(Species& species, Substitution& substitution) {

            }
        public:
            Substitution Optimize() {
                // I. Generate initial landscape

                Species winner;
                random_selector<> selector;

                std::default_random_engine engine{std::random_device{}()};
                std::uniform_real_distribution<double> distribution (0,1);

                // II. Evolve
                while (true) {
                    // Generate mutations
                    {
                        std::vector<Species> mutations;

                        // For every species, generate a random mutation
                        for (auto &e : elements) {
                            // Randomly search for an expression
                            auto expr = *selector(e.expressions.begin(), e.expressions.end());
                            Substitution subst;

                            Species newSpecies = e;

                            // If not addition, can only rescale
                            if (!expr.IsAdded()) {
                                if (!expr.IsMultiplied()) continue;

                                ScalarMutation(newSpecies, subst);
                            } else {
                                if (distribution(engine) < 0.5) {
                                    AdditiveMutation(newSpecies, expr, subst);
                                } else ScalarMutation(newSpecies, expr, subst);
                            }

                            newSpecies.substitution = Substitution::Merge({ subst, newSpecies.substitution });
                            mutations.push_back(newSpecies);
                        }

                        // Add mutations to the landscape
                        for (auto& e : mutations) {
                            elements.push_back(e);
                        }
                    }

                    // Sort by the fitness of the species
                    std::sort(elements.begin(), elements.end(), [](const Species& a, const Species& b) {
                        return CostFunction(a) < CostFunction(b);
                    });

                    // Kill all wimps
                    elements.resize(numElements);

                    // If we found a new winner, reset the counter and note the winner
                    if (winner != elements[0]) {
                        unchangedIterations = 0;
                        winner = elements[0];
                        continue;
                    }

                    // Increase the number of unchanged iterations by 1
                    ++unchangedIterations;

                    // If the winner has not changed for 1000 generations, terminate
                    if (unchangedIterations > 1000) {
                        break;
                    }
                }

                // III. Return the result
                return elements[0].substitution;
            }
        private:
            unsigned numElements;
            unsigned unchangedIterations;

            std::vector<Species> elements;
        };

    }
}