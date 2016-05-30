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
                auto combinations = tensors[0].GetAllIndexCombinations();

                // Derive vector components of the tensors
                //std::vector<std::thread> threads;

                auto fn = [&](const Tensor::TensorPointer& tensor) {
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

                        v[i] = (*tensor)(assignment);
                    }

                    vectors.push_back(v);
                };

                // Emplace the threads into the vector
                for (auto& tensor : tensors) {
                    fn(tensor);
                    // threads.emplace_back(std::thread(fn, tensor));
                }

                // Join
                /*for (auto& thread : threads) {
                    thread.join();
                }*/

                // Create matrix
                Matrix M (vectors);

                // Reduce to matrix echelon form
                M.ToRowEchelonForm();

                // Select all the linear independent tensors from M and put into a new container
                TensorContainer result;
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
                            result.Insert(tensors.Get(i));
                        }
                    } else if (!isUnitVector) {
                        std::unique_lock<std::mutex> lock(mutex);

                        previous.push_back(vec);
                        result.Insert(tensors.Get(i));
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