#pragma once

#include <vector/vector.hpp>
#include <vector/matrix.hpp>

#include <iostream>

#include <tensor/tensor.hpp>
#include <tensor/tensor_container.hpp>

using Construction::Tensor::Tensor;
using Construction::Tensor::TensorContainer;
using Construction::Vector::Vector;
using Construction::Vector::Matrix;

namespace Construction {
    namespace Generator {

        /**
            \class BasisSelector
         */
        class LinearDependentSelector {
        public:
            typedef std::pair<double, TensorPointer>            BasisReferenceType;
            typedef std::vector<BasisReferenceType>             DependenceType;
            typedef std::pair<TensorPointer, DependenceType>    DependentReferenceType;
            typedef std::vector<DependentReferenceType>         ResultType;
        public:
            /**

             */
            TensorContainer SwapDependencyInformation(const ResultType& deps) {
                TensorContainer result;

                for (auto& dependency: deps) {
                    result.Insert(dependency.first);
                }

                return result;
            }

            TensorPointer DependencyToTensor(const DependenceType& dep) {
                if (dep.size() == 0) return nullptr;

                TensorPointer current = dep[0].second->Clone();
                for (int i=1; i<dep.size(); ++i) {
                    current = std::make_shared<Tensor::AddedTensor>(
                        std::move(current),
                        std::make_shared<Tensor::ScaledTensor>(dep[i].second->Clone(), dep[i].first)
                    );
                }

                return current;
            }

            bool IsCorrect(const DependentReferenceType& ref) {
                if (ref.second.size() == 0) {
                    if (ref.first->IsZero()) return true;
                    return false;
                }

                auto other = DependencyToTensor(ref.second);
                auto combinations = other->GetAllIndexCombinations();

                for (auto& combination : combinations) {
                    if (other->Evaluate(combination) != ref.first->Evaluate(combination)) return false;
                }

                return true;
            }

            bool IsCorrect(const ResultType& res) {
                for (auto& r : res) {
                    if (!IsCorrect(r)) return false;
                }
                return true;
            }

            /**
                \brief Select all the linear dependent tensors from the given tensors
             */
            ResultType operator()(const TensorContainer& tensors) const {
                if (tensors.Size() == 0) return ResultType();

                // Initialize
                std::vector<Vector::Vector> vectors;

                // Get first
                auto indices = tensors[0].GetIndices();

                // Derive vector components of the tensors
                for (auto& tensor : tensors) {
                    auto combinations = tensor->GetAllIndexCombinations();

                    // TODO: implement optimization for all indices that are trivially zero
                    Vector::Vector v (combinations.size());
                    for (int i=0; i<combinations.size(); i++) {
                        Tensor::IndexAssignments assignment;

                        // Convert into index assignment
                        int j=0;
                        for (auto& index : indices) {
                            assignment[index.GetName()] = combinations[i][j];
                            j++;
                        }

                        v[i] = (*tensor)(assignment);
                    }

                    vectors.push_back(v);
                }

                // Create matrix
                Matrix M (vectors);

                // Reduce to matrix echelon form
                M.ToRowEchelonForm();

                //std::cout << M << std::endl;

                // Select all the linear independent tensors from M and put into a new container
                ResultType result;

                std::vector<Vector::Vector> columns;
                for (int i=0; i<M.GetNumberOfColumns(); i++) {
                    auto vec = M.GetColumnVector(i);

                    DependenceType coeffs;

                    bool firstNumber = false;
                    bool linearDependent = false;
                    for (int j=0; j<vec.GetDimension(); j++) {
                        if (vec[j] != 0.0) {
                            coeffs.push_back({ vec[j], tensors.Get(j) });

                            if (!firstNumber && vec[j] == 1) {
                                firstNumber = true;
                                continue;
                            }

                            // If this is reached, the vector is linear dependent
                            linearDependent = true;
                        }
                    }

                    // Add the coefficients to the result
                    if (linearDependent) result.push_back({tensors.Get(i), coeffs});
                }

                return result;

                /*for (int i=0; i<vectors[0].GetDimension(); i++) {
                    Vector::Vector v (vectors[0].GetDimension());
                    v[i] = 1.0;

                    // Look for the vector
                    auto it = std::find(columns)
                }*/

                // Check all the unit vectors for the different combinations
                /*for (int i=0; i<vectors[0].GetDimension(); i++) {
                    Vector::Vector v (vectors[0].GetDimension());
                    v[i] = 1.0;

                    // Look for the vector
                    auto it = std::find(columns.begin(), columns.end(), v);
                    // If found, add the corresponding tensor to the output
                    if (it != columns.end()) {
                        auto pos = std::distance(columns.begin(), it);
                        result.Insert(tensors.Get(pos));
                    }
                }*/

                return result;
            }
        };

    }
}