#pragma once

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>

#include <common/time_measurement.hpp>

#include <generator/base_tensor.hpp>
#include <generator/symmetrized_tensor.hpp>

using Construction::Tensor::Tensor;
using Construction::Tensor::Scalar;
using Construction::Tensor::Indices;

namespace Construction {
    namespace Language {

        namespace API {

            /**
                API for the language

                `Arbitrary`
                `Symmetrize`
                `Apppend`
                `Evaluate`
                `LinearIndependent`
                `LinearDependent`
             */

            // Tensor::Tensor generation
            Tensor::Tensor Arbitrary(const Indices& indices);
            Tensor::Tensor Epsilon(const Indices& indices);
            Tensor::Tensor Gamma(const Indices& indices);
            Tensor::Tensor EpsilonGamma(const Indices& indices);
            Tensor::Tensor Coefficient(unsigned, unsigned, unsigned, unsigned);

            size_t DegreesOfFreedom(const Tensor::Tensor& tensor);

            Tensor::Tensor Symmetrize(const Tensor::Tensor& tensor, const Indices& indices);
            Tensor::Tensor AntiSymmetrize(const Tensor::Tensor& tensor, const Indices& indices);
            Tensor::Tensor BlockSymmetrize(const Tensor::Tensor& tensor, const std::vector<Indices>& indices);

            Tensor::Tensor Expand(const Tensor::Tensor& tensor);
            Tensor::Tensor Simplify(const Tensor::Tensor& tensor);
            Tensor::Tensor RedefineVariables(const Tensor::Tensor& tensor);

            Tensor::Tensor Add(const Tensor::Tensor& first, const Tensor::Tensor& second);
            Tensor::Tensor Scale(const Tensor::Tensor& first, const Scalar& scalar);
            Tensor::Tensor Multiply(const Tensor::Tensor& first, const Tensor::Tensor& second);

            bool IsSymmetric(const Tensor::Tensor& tensor, const Indices& indices);
            bool IsAntiSymmetric(const Tensor::Tensor& tensor, const Indices& indices);
            bool IsBlockSymmetric(const Tensor::Tensor& tensor, const std::vector<Indices>& indices);

            std::vector<Tensor::Tensor> LinearIndependent(const std::vector<Tensor::Tensor>& tensors);
            std::vector<std::pair<Tensor::Tensor,Tensor::Tensor>> LinearDependent(const std::vector<Tensor::Tensor>& tensors);

            std::vector<std::pair<Scalar, Scalar>> HomogeneousSystem(const Tensor::Tensor& tensor);

            Scalar Evaluate(const Tensor::TensorContainer& tensors, const std::vector<unsigned>& indices);

            /**
                Implementation
             */
            Tensor::Tensor Arbitrary(const Indices& indices) {
                Generator::BaseTensorGenerator generator;
                return generator.Generate(indices);
            }

            Tensor::Tensor Epsilon(const Indices& indices) {
                // Assert that we have as many indices as the dimension
                assert(indices.Size() == indices[0].GetRange().GetDimension());
                return Tensor::Tensor::Epsilon(indices);
            }

            Tensor::Tensor Gamma(const Indices& indices) {
                return Tensor::Tensor::Gamma(indices);
            }

            Tensor::Tensor EpsilonGamma(const Indices& indices) {
                // Calculate the numbers of epsilon and gammas
                unsigned N = indices.Size();
                unsigned numEpsilon = (N % 2 == 1) ? 1 : 0;
                unsigned numGamma   = (N % 2 == 1) ? (N-3)/2 : N/2;

                // Generate a epsilon gamma Tensor::Tensor
                return Tensor::Tensor::EpsilonGamma(numEpsilon, numGamma, indices);
            }

            //#include <generator/coefficient.hpp

            Tensor::Tensor Coefficient(unsigned l, unsigned ld, unsigned r, unsigned rd) {
                return Tensor::Tensor::Zero();
                /*Generator::CoefficientGenerator generator(nullptr);
                return generator.Generate(l,ld,r,rd);*/
            }

            /*Tensor::TensorContainer Append(const Tensor::TensorContainer& first, const Tensor::TensorContainer& second) {
                Tensor::TensorContainer result = first;

                for (auto& f : second) {
                    result.Insert(f);
                }

                return result;
            }*/

            size_t DegreesOfFreedom(const Tensor::Tensor& tensor) {
                // TODO: really count the number of variables
                return tensor.GetSummands().size();
            }

            Tensor::Tensor Symmetrize(const Tensor::Tensor& tensor, const Indices& indices) {
                Common::TimeMeasurement time;

                // Do the symmetrization with the generator
                Construction::Generator::SymmetrizedTensorGenerator symmetrizer(indices);
                auto result = symmetrizer(tensor);

                time.Stop();
                //std::cout << "  \033[90m" << time << "\033[0m" << std::endl;

                return result;
            }

            Tensor::Tensor AntiSymmetrize(const Tensor::Tensor& tensor, const Indices& indices) {
                // Get the original indices
                Construction::Generator::AntiSymmetrizedTensorGenerator symmetrizer(indices);
                auto result = symmetrizer(tensor);

                return result;
            }

            Tensor::Tensor ExchangeSymmetrize(const Tensor::Tensor& tensor, const Indices& indices) {
                Common::TimeMeasurement time;

                // Get the original indices
                Construction::Generator::ExchangeSymmetrizedTensorGenerator symmetrizer(indices);
                auto result = symmetrizer(tensor);

                time.Stop();
                //std::cout << "  \033[90m" << time << "\033[0m" << std::endl;

                return result;
            }

            Tensor::Tensor BlockSymmetrize(const Tensor::Tensor& tensors, const std::vector<Indices>& blocks) {
                // TODO: implement
                return tensors;

                //Construction::Generator::BlockSymmetrizedTensorGenerator symmetrizer(blocks);
                //auto result = symmetrizer(tensors);
                //return result;
            }

            Tensor::Tensor Expand(const Tensor::Tensor& tensor) {
                return tensor.Expand();
            }

            Tensor::Tensor Simplify(const Tensor::Tensor& tensor) {
                return tensor.Simplify();
            }

            Tensor::Tensor RedefineVariables(const Tensor::Tensor& tensor) {
                return tensor.RedefineVariables("e");
            }

            bool IsSymmetric(const Tensor::Tensor& tensor, const Indices& indices) {
                Construction::Generator::SymmetrizedTensorGenerator symmetrizer(indices);
                return symmetrizer(tensor, true).IsEqual(tensor);
            }

            bool IsAntiSymmetric(const Tensor::Tensor& tensor, const Indices& indices) {
                Construction::Generator::AntiSymmetrizedTensorGenerator symmetrizer(indices);
                return symmetrizer(tensor, true).IsEqual(tensor);
            }

            bool IsBlockSymmetric(const Tensor::Tensor& tensor, const Indices& indices) {
                // TODO: implement this
                return false;
                //Construction::Generator::BlockSymmetrizedTensorGenerator symmetrizer(indices);
                //return symmetrizer(tensor, true).IsEqual(tensor);
            }

            Tensor::Tensor Add(const Tensor::Tensor& first, const Tensor::Tensor& second) {
                return first + second;
            }

            Tensor::Tensor Scale(const Tensor::Tensor& first, const Scalar& c) {
                return first * c;
            }

            Tensor::Tensor Multiply(const Tensor::Tensor& first, const Tensor::Tensor& second) {
                return first * second;
            }

            std::vector<Tensor::Tensor> LinearIndependent(const std::vector<Tensor::Tensor>& tensors) {
                // First sum all the Tensor::Tensors
                Tensor::Tensor tensor = Tensor::Tensor::Zero();
                for (auto& t : tensors) tensor += t;

                // Simplify and thus get rid of all the linear dependent ones
                // The linear independent ones are the remaining ones
                // TODO: keep the original scale of the tensor
                tensor = tensor.Simplify();

                return tensor.GetSummands();
            }

            std::vector<std::pair<Tensor::Tensor,Tensor::Tensor>> LinearDependent(const std::vector<Tensor::Tensor>& tensors) {
                // TODO: implement this
                std::vector<std::pair<Tensor::Tensor,Tensor::Tensor>> result;
                return result;
            }

            std::vector<std::pair<Scalar, Scalar>> HomogeneousSystem(const Tensor::Tensor& tensor) {
                auto system = tensor.ToHomogeneousLinearSystem();

                // Reduce
                system.first.ToRowEchelonForm();

                std::vector<std::pair<Scalar, Scalar>> result;

                // Extract the results
                for (int i=0; i<system.first.GetNumberOfRows(); i++) {
                    auto vec = system.first.GetRowVector(i);

                    // If the vector has zero norm, we get no further information => quit
                    if (vec * vec == 0) break;

                    bool isZero = true;
                    Scalar lhs = 0;
                    Scalar rhs = 0;

                    // Iterate over all the components
                    for (int j=0; j<vec.GetDimension(); j++) {
                        if (vec[j] == 0 && isZero) continue;
                        if (vec[j] == 1 && isZero) {
                            lhs = system.second[j];
                            isZero = false;
                        } else if (vec[j] != 0) {
                            rhs += (-system.second[j] * vec[j]);
                        }
                    }

                    // Add to the result
                    result.push_back({lhs, rhs});
                }

                return result;
            }

            Scalar Evaluate(const Tensor::Tensor& tensor, const std::vector<unsigned>& indices) {
                return tensor(indices);
            }

        }

    }
}
