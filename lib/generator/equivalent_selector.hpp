#pragma once

#include <vector/vector.hpp>
#include <vector/matrix.hpp>

#include <iostream>

#include <tensor/tensor.hpp>

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
            std::vector<Tensor::Tensor> operator()(const std::vector<Tensor::Tensor>& tensors) const {
                if (tensors.size() == 0) return tensors;

                // Initialize
                std::vector<Vector::Vector> vectors;

                // Get first
                auto indices = tensors[0].GetIndices();
                auto combinations = tensors[0].GetAllIndexCombinations();

                // Derive vector components of the tensors
                //std::vector<std::thread> threads;

                auto fn = [&](const Tensor::Tensor& tensor) {
                    // TODO: implement optimization for all indices that are trivially zero
                    Vector::Vector v (combinations.size());
                    for (int i=0; i<combinations.size(); i++) {
                        Tensor::IndexAssignments assignment;

                        // Convert into index assignment
                        int j = 0;
                        for (auto &index : indices) {
                            assignment[index.GetName()] = combinations[i][j];
                            j++;
                        }

                        v[i] = tensor(assignment);
                    }

                    vectors.push_back(v);
                };

                // Emplace the threads into the vector
                for (auto& tensor : tensors) {
                    // If the tensor is multiplied by a free variable, I can always redefine 
                    // it to be the whole expression in front of the tensor and thus
                    // it does not play a role when looking for equivalent structure
                    if (tensor.IsScaled() && tensor.HasVariables()) {
                        fn(tensor.SeparateScalefactor().second);
                    } else {
                        fn(tensor);
                    }
                }

                // Join
                /*for (auto& thread : threads) {
                    thread.join();
                }*/

                // Create matrix
                Vector::Matrix M (vectors);

                // Reduce to matrix echelon form
                M.ToRowEchelonForm();

                // Select all the linear independent tensors from M and put into a new container
                std::vector<Tensor::Tensor> result;
                std::mutex mutex;
                std::vector<Vector::Vector> previous;
                //std::vector<std::thread> threads2;

                auto fn2 = [&](int i) {
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
                        std::unique_lock<std::mutex> lock(mutex);

                        // If this tensor is not in the list already, add it
                        if (std::find(previous.begin(), previous.end(), vec) == previous.end()) {
                            previous.push_back(vec);
                            result.push_back(tensors[i]);
                        }
                    } else if (!isUnitVector) {
                        std::unique_lock<std::mutex> lock(mutex);

                        previous.push_back(vec);
                        result.push_back(tensors[i]);
                    }
                };

                for (int i=0; i<M.GetNumberOfColumns(); i++) {
                    fn2(i);
                    //threads2.emplace_back(std::thread(std::bind(fn2, i)));
                }

                /*for (auto& thread : threads2) {
                    thread.join();
                }*/

                return result;
            }
        };

    }
}