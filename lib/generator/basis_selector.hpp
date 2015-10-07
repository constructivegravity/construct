#pragma once

#include <vector/vector.hpp>
#include <vector/matrix.hpp>

#include <tensor/tensor.hpp>
#include <tensor/tensor_container.hpp>

using Albus::Tensor::Tensor;
using Albus::Tensor::TensorContainer;
using Albus::Vector::Vector;
using Albus::Vector::Matrix;

namespace Albus {
    namespace Generator {

        /**
            \class BasisSelector
         */
        class BasisSelector {
        public:
            /**
                \brief Select all the linear dependent tensors from the given tensors
             */
            TensorContainer operator()(const TensorContainer& tensors) const {
                // Initialize
                std::vector<Vector> vectors;

                // Derive vector components of the tensors
                for (auto& tensor : tensors) {
                    auto combinations = tensor.GetAllIndexCombinations();
                    // TODO: implement optimization for all indices that are trivially zero

                    Vector::Vector v (combinations.size());
                    for (int i=0; i<combinations.size(); i++) {
                        v[i] = tensor(combinations[i]);
                    }
                    vectors.push_back(std::move(v));
                }

                // Create matrix
                Matrix M (vectors);

                // Reduce to matrix echelon form
                M.ToRowEchelonForm();

                // Select all the linear independent tensors from M and put into a new container
                TensorContainer result;
                return result;
            }
        };

    }
}