#pragma once

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/tensor_container.hpp>

#include <generator/base_tensor.hpp>
#include <generator/equivalent_selector.hpp>
#include <generator/basis_selector.hpp>
#include <generator/linear_dependent_selector.hpp>
#include <generator/symmetrized_tensor.hpp>

namespace Construction {
    namespace Language {

        namespace API {

            /**
                API for the language

                `Tensor`
                `Symmetrize`
                `Apppend`
                `LinearIndependent`
                `LinearDependent`
             */

            // Tensor generation
            TensorContainer Tensor(const Indices& indices);
            TensorContainer EpsilonGamma(const Indices& indices);
            TensorContainer Append(const TensorContainer& first, const TensorContainer& second);

            size_t DegreesOfFreedom(const TensorContainer& tensors);

            // Symmetrization
            TensorContainer Symmetrize(const TensorContainer& tensors, const Indices& indices);
            TensorContainer AntiSymmetrize(const TensorContainer& tensors, const Indices& indices);
            TensorContainer BlockSymmetrize(const TensorContainer& tensors, const std::vector<Indices>& indices);

            TensorContainer LinearIndependent(const TensorContainer& tensors);
            Generator::LinearDependentSelector::ResultType LinearDependent(const TensorContainer& tensors);



            /**
                Implementation
             */



            TensorContainer Tensor(const Indices& indices) {
                Generator::BaseTensorGenerator generator;

                // Construct tensor from the indices
                return generator.Generate(indices);
            }

            TensorContainer EpsilonGamma(const Indices& indices) {
                // Calculate the numbers of epsilon and gammas
                unsigned N = indices.Size();
                unsigned numEpsilon = (N % 2 == 1) ? 1 : 0;
                unsigned numGamma   = (N % 2 == 1) ? (N-3)/2 : N/2;

                // Generate a epsilon gamma tensor and insert it into the container
                TensorContainer result;
                result.Insert(std::make_shared<Tensor::EpsilonGammaTensor>(numEpsilon, numGamma, indices));

                return result;
            }

            TensorContainer Append(const TensorContainer& first, const TensorContainer& second) {
                TensorContainer result = first;

                for (auto& f : second) {
                    result.Insert(f);
                }

                return result;
            }

            size_t DegreesOfFreedom(const TensorContainer& tensors) {
                return tensors.Size();
            }

            TensorContainer Symmetrize(const TensorContainer& tensors, const Indices& indices) {
                // Get the original indices
                Construction::Generator::SymmetrizedTensorGenerator symmetrizer(indices);
                auto result = symmetrizer(tensors);

                return result;
            }

            TensorContainer AntiSymmetrize(const TensorContainer& tensors, const Indices& indices) {
                // Get the original indices
                Construction::Generator::AntiSymmetrizedTensorGenerator symmetrizer(indices);
                auto result = symmetrizer(tensors);

                return result;
            }

            TensorContainer BlockSymmetrize(const TensorContainer& tensors, const std::vector<Indices>& blocks) {
                // TODO: implement
                return TensorContainer();
            }

            TensorContainer LinearIndependent(const TensorContainer& tensors) {
                Construction::Generator::BasisSelector selector;
                auto result = selector(tensors);
                return result;
            }

            Generator::LinearDependentSelector::ResultType LinearDependent(const TensorContainer& tensors) {
                Construction::Generator::LinearDependentSelector selector;
                auto result = selector(tensors);
                return result;
            }

        }

    }
}