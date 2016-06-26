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
        class BasisSelector {
        public:
            /**
                \brief Select all the linear independent tensors from the given tensors
             */
            std::vector<Tensor::Tensor> operator()(const std::vector<Tensor::Tensor>& tensors) const {
                if (tensors.size() == 0) return tensors;

                // Initialize
                std::vector<Vector::Vector> vectors;

                //std::cout << tensors << std::endl;

                // Get first
                auto indices = tensors[0].GetIndices();

                // Derive vector components of the tensors
                for (auto& tensor : tensors) {
                    auto combinations = tensor.GetAllIndexCombinations();

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

                        v[i] = tensor(assignment);
                    }

                    vectors.push_back(v);
                }

                // Create matrix
                Vector::Matrix M (vectors);

                // Reduce to matrix echelon form
                M.ToRowEchelonForm();

                //std::cout << M << std::endl;

                // Select all the linear independent tensors from M and put into a new container
                std::vector<Tensor::Tensor> result;

                std::vector<Vector::Vector> columns;
                for (int i=0; i<M.GetNumberOfColumns(); i++) {
                    auto vec = M.GetColumnVector(i);
                    columns.push_back(vec);
                }

                // Check all the unit vectors for the different combinations
                for (int i=0; i<vectors[0].GetDimension(); i++) {
                    Vector::Vector v (vectors[0].GetDimension());
                    v[i] = 1.0;

                    // Look for the vector
                    auto it = std::find(columns.begin(), columns.end(), v);
                    // If found, add the corresponding tensor to the output
                    if (it != columns.end()) {
                        auto pos = std::distance(columns.begin(), it);
                        result.push_back(tensors[pos]);
                    }
                }

                return result;
            }
        };

    }
}