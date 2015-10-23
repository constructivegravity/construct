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
        class EquivalentSelector {
        public:
            /**
                \brief Select all the linear dependent tensors from the given tensors
             */
            TensorContainer operator()(const TensorContainer& tensors) const {
                if (tensors.Size() == 0) return TensorContainer();

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

                // Select all the linear independent tensors from M and put into a new container
                TensorContainer result;

                std::vector<Vector::Vector> previous;
                for (int i=0; i<M.GetNumberOfColumns(); i++) {
                    auto vec = M.GetColumnVector(i);

                    bool firstNumber = false;
                    bool isUnitVector = true;
                    for (int j=0; j<vec.GetDimension(); j++) {
                        if (vec[j] != 0.0) {
                            if (!firstNumber && vec[j] == 1) {
                                firstNumber = true;
                                continue;
                            }

                            isUnitVector = false;
                            break;
                        }
                    }

                    if (firstNumber && isUnitVector) {
                        // If this tensor is not in the list already, add it
                        if (std::find(previous.begin(), previous.end(), vec) == previous.end()) {
                            previous.push_back(vec);
                            result.Insert(tensors.Get(i));
                        }
                    } else if (!isUnitVector) {
                        previous.push_back(vec);
                        result.Insert(tensors.Get(i));
                    }
                }

                return result;
            }
        };

    }
}