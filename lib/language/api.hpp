#pragma once

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/tensor_container.hpp>

#include <common/time_measurement.hpp>

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
            TensorContainer Coefficient(int, int, int, int);
            TensorContainer Append(const TensorContainer& first, const TensorContainer& second);

            size_t DegreesOfFreedom(const TensorContainer& tensors);

            // Symmetrization
            TensorContainer Symmetrize(const TensorContainer& tensors, const Indices& indices);
            TensorContainer AntiSymmetrize(const TensorContainer& tensors, const Indices& indices);
            TensorContainer BlockSymmetrize(const TensorContainer& tensors, const std::vector<Indices>& indices);

            TensorPointer Add(const TensorPointer& first, const TensorPointer& second);
            TensorPointer Scale(const TensorPointer& first, double number);

            bool IsSymmetric(const TensorPointer& tensors, const Indices& indices);
            bool IsAntiSymmetric(const TensorPointer& tensors, const Indices& indices);
            bool IsBlockSymmetric(const TensorPointer& tensors, const std::vector<Indices>& indices);

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

//#include <generator/coefficient.hpp

            TensorContainer Coefficient(int l, int ld, int r, int rd) {
                return TensorContainer();
                /*Generator::CoefficientGenerator generator(nullptr);
                return generator.Generate(l,ld,r,rd);*/
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
                Common::TimeMeasurement time;

                // Get the original indices
                Construction::Generator::SymmetrizedTensorGenerator symmetrizer(indices);
                auto result = symmetrizer(tensors);

                time.Stop();
                //std::cout << "  \033[90m" << time << "\033[0m" << std::endl;

                return result;
            }

            TensorContainer AntiSymmetrize(const TensorContainer& tensors, const Indices& indices) {
                // Get the original indices
                Construction::Generator::AntiSymmetrizedTensorGenerator symmetrizer(indices);
                auto result = symmetrizer(tensors);

                return result;
            }

            TensorContainer ExchangeSymmetrize(const TensorContainer& tensors, const Indices& indices) {
                Common::TimeMeasurement time;

                // Get the original indices
                Construction::Generator::ExchangeSymmetrizedTensorGenerator symmetrizer(indices);
                auto result = symmetrizer(tensors);

                time.Stop();
                //std::cout << "  \033[90m" << time << "\033[0m" << std::endl;

                return result;
            }

            TensorContainer BlockSymmetrize(const TensorContainer& tensors, const std::vector<Indices>& blocks) {
                Construction::Generator::BlockSymmetrizedTensorGenerator symmetrizer(blocks);
                auto result = symmetrizer(tensors);
                return result;
            }

            bool IsSymmetric(const TensorPointer& tensor, const Indices& indices) {
                // Get the original indices
                Construction::Generator::SymmetrizedTensorGenerator symmetrizer(indices);

                // Insert into container
                TensorContainer container;
                container.Insert(tensor);

                // Symmetrize
                auto result = symmetrizer(container, true);

                // If no tensor is returned, then the original one
                // is antisymmetric in the given indices, so return false
                if (result.Size() == 0) return false;

                // Compare if the symmetrized tensor is identical to the original
                return result.Get(0)->IsEqual(*tensor);
            }

            bool IsAntiSymmetric(const TensorPointer& tensor, const Indices& indices) {
                // Get the original indices
                Construction::Generator::AntiSymmetrizedTensorGenerator symmetrizer(indices);

                // Insert into container
                TensorContainer container;
                container.Insert(tensor);

                // Symmetrize
                auto result = symmetrizer(container, true);

                // If no tensor is returned, then the original one
                // is antisymmetric in the given indices, so return false
                if (result.Size() == 0) return false;

                // Compare if the symmetrized tensor is identical to the original
                return result.Get(0)->IsEqual(*tensor);
            }

            bool IsBlockSymmetric(const TensorPointer& tensor, const std::vector<Indices>& indices) {
                // Get the original indices
                Construction::Generator::BlockSymmetrizedTensorGenerator symmetrizer(indices);

                // Insert into container
                TensorContainer container;
                container.Insert(tensor);

                // Symmetrize
                auto result = symmetrizer(container, true);

                // If no tensor is returned, then the original one
                // is antisymmetric in the given indices, so return false
                if (result.Size() == 0) return false;

                // Compare if the symmetrized tensor is identical to the original
                return result.Get(0)->IsEqual(*tensor);
            }

            TensorPointer Add(const TensorPointer& first, const TensorPointer& second) {
                return std::make_shared<Construction::Tensor::AddedTensor> (
                    std::move(first->Clone()),
                    std::move(second->Clone())
                );
            }

            TensorPointer Scale(const TensorPointer& first, double number) {
                return std::make_shared<Construction::Tensor::ScaledTensor> (
                    std::move(first->Clone()),
                    number
                );
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
