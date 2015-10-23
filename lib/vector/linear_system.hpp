#pragma once

#include <vector/vector.hpp>
#include <vector/matrix.hpp>

namespace Construction {
    namespace Vector {

        // Forward declaration
        template<typename F, class V>
        class LinearSystem;

        /**
            \class LinearElement


         */
        template<class F, class V>
        class LinearElement {
        public:
            LinearElement(LinearSystem<F,V>* system);
        public:
            friend std::ostream& operator<<(std::ostream& os, const LinearElement& el) {
                int i=0;
                for (auto& basis : *el.system) {
                    os << " " << el.components[i] << "*" << basis;
                    i++;
                }
                return os;
            }
        private:
            std::vector<F> components;
            LinearSystem<F,V>* system;
        };

        /**
            \class LinearSystem

            Implements the abstract notion of a vector space. The field is given by
            template and so is the principle class for the vector elements.
            It allows to add an arbitrary amount of
         */
        template<typename F, class V>
        class LinearSystem {
        public:
            typedef std::shared_ptr<V>                                          VectorPointer;
            typedef std::shared_ptr<const V>                                    ConstVectorPointer;

            typedef std::function<Vector::Vector(const ConstVectorPointer&)>    EvaluationFunctor;
            typedef std::vector<F>                                              Components;

            typedef std::map<V, std::vector<double>>                            DecompositionCache;
        public:
            LinearSystem() : evaluator([&](const ConstVectorPointer&) -> Vector::Vector {
                return Vector::Vector(GetDimension());
            }) { }

            LinearSystem(const std::vector<ConstVectorPointer>& basis) : basis(basis), evaluator([&](const ConstVectorPointer&) -> Vector::Vector {
                return Vector::Vector(GetDimension());
            }) { }
            LinearSystem(std::vector<ConstVectorPointer>&& basis) : basis(std::move(basis)), evaluator([&](const ConstVectorPointer&) -> Vector::Vector {
                return Vector::Vector(GetDimension());
            }) { }

            LinearSystem(const std::vector<ConstVectorPointer>& basis, const EvaluationFunctor& fn) : basis(basis), evaluator(fn) { }
            LinearSystem(std::vector<ConstVectorPointer>&& basis, const EvaluationFunctor& fn) : basis(std::move(basis)), evaluator(fn) { }

            LinearSystem(const LinearSystem& other) : basis(other.basis), evaluator(other.evaluator), cache(other.cache) { }
            LinearSystem(LinearSystem&& other) : basis(std::move(other.basis)), evaluator(std::move(other.evaluator)), cache(std::move(other.cache)) { }
        public:
            size_t GetDimension() {
                if (isReduced)
                    return basis.size();
                else return 0;
            }
        public:
            void AddToBasis(const ConstVectorPointer& pointer) {
                basis.push_back(pointer);
                isReduced = false;
            }
        private:
            bool IsNewBasisVector(const Vector::Vector vec) const {
                bool onlyZeroes = true;
                int k = 0;

                // Iterate over all the components in the vector
                for (int i=0; i<vec.GetDimension(); i++) {
                    if (vec[i] != 0) {
                        // if this is the first element that is exactly one set onlyZero to true
                        if (!onlyZeroes && vec[i] == 1) {
                            onlyZeroes = true;
                            k = i;

                        }
                        else return false;
                    }
                }

                // Check if the basis element is not in the basis so far
                {
                    // Lock the mutex
                    std::unique_lock<std::mutex> lock(basisMutex);

                    if (k < basis.size()) return false;
                }

                return true;
            }
        public:
            void ReduceBasis() {
                if (basis.size() == 0) return;

                // Copy the elements and clear the basis and cache
                std::vector<ConstVectorPointer> copy = basis;
                basis.clear();
                cache.clear();

                // Create numerical vectors
                std::vector<Vector::Vector> vectors;

                // Iterate over all elements in the "basis"
                for (auto& vec : copy) {
                    vectors.push_back(evaluator(vec));
                }

                // Create matrix
                Matrix M (vectors);

                std::cout << M << std::endl;

                // Reduce to matrix echelon form
                M.ToRowEchelonForm();

                // Iterate over all columns
                for (int i=0; i<M.GetNumberOfColumns(); ++i) {
                    auto vec = M.GetColumnVector(i);

                    // Check if is a basis vector
                    if (IsNewBasisVector(vec)) {
                        std::unique_lock<std::mutex> lock(basisMutex);

                        basis.push_back(std::move(copy[i]));
                    }
                    // else insert this with its components
                    // into the cache
                    else {
                        std::vector<double> components;
                        for (auto& i : vec) {
                            components.push_back(i);
                        }
                        cache[copy[i]] = components;
                    }
                }

                isReduced = true;
            }
        public:
            typename std::vector<ConstVectorPointer>::iterator begin() { return basis.begin(); }
            typename std::vector<ConstVectorPointer>::iterator end() { return basis.end(); }

            typename std::vector<ConstVectorPointer>::const_iterator begin() const { return basis.begin(); }
            typename std::vector<ConstVectorPointer>::const_iterator end() const { return basis.end(); }
        private:
            std::mutex basisMutex;

            std::vector<typename ConstVectorPointer> basis;

            EvaluationFunctor evaluator;
            DecompositionCache cache;

            bool isReduced=true;
        };

        LinearElement::LinearElement(LinearSystem *system) : system(system) {
            components.resize(system->GetDimension());
        }

    }
}