#pragma once

#include <common/logger.hpp>

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/substitution.hpp>

#include <common/time_measurement.hpp>

#include <generator/base_tensor.hpp>

using Construction::Tensor::Tensor;
using Construction::Tensor::Scalar;
using Construction::Tensor::Indices;
using Construction::Tensor::Substitution;

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
            Tensor::Tensor InverseEpsilon(const Indices& indices);
            Tensor::Tensor Gamma(const Indices& indices);
            Tensor::Tensor InverseGamma(const Indices& indices);
            Tensor::Tensor EpsilonGamma(const Indices& indices);
            Tensor::Tensor Delta(const Indices& indices);
            Tensor::Tensor Coefficient(unsigned, unsigned, unsigned, unsigned);

            size_t DegreesOfFreedom(const Tensor::Tensor& tensor);

            Tensor::Tensor Symmetrize(const Tensor::Tensor& tensor, const Indices& indices);
            Tensor::Tensor AntiSymmetrize(const Tensor::Tensor& tensor, const Indices& indices);
            Tensor::Tensor BlockSymmetrize(const Tensor::Tensor& tensor, const std::vector<Indices>& indices);

            Tensor::Tensor Expand(const Tensor::Tensor& tensor);
            Tensor::Tensor Simplify(const Tensor::Tensor& tensor);
            Tensor::Tensor RedefineVariables(const Tensor::Tensor& tensor);
            Tensor::Tensor RenameIndices(const Tensor::Tensor& tensor, const Indices& from, const Indices& to);

            Tensor::Tensor Add(const Tensor::Tensor& first, const Tensor::Tensor& second);
            Tensor::Tensor Scale(const Tensor::Tensor& first, const Scalar& scalar);
            Tensor::Tensor Multiply(const Tensor::Tensor& first, const Tensor::Tensor& second);

            bool IsSymmetric(const Tensor::Tensor& tensor, const Indices& indices);
            bool IsAntiSymmetric(const Tensor::Tensor& tensor, const Indices& indices);
            bool IsBlockSymmetric(const Tensor::Tensor& tensor, const std::vector<Indices>& indices);

            std::vector<Tensor::Tensor> LinearIndependent(const std::vector<Tensor::Tensor>& tensors);
            std::vector<std::pair<Tensor::Tensor,Tensor::Tensor>> LinearDependent(const std::vector<Tensor::Tensor>& tensors);

            Substitution HomogeneousSystem(const Tensor::Tensor& tensor);

            Tensor::Tensor Substitute(const Tensor::Tensor& tensor, const Substitution& substitution);

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

            Tensor::Tensor InverseEpsilon(const Indices& indices) {
                // Assert that we have as many indices as the dimension
                assert(indices.Size() == indices[0].GetRange().GetDimension());

                auto clone = indices;

                clone[0].SetContravariant(true);
                clone[1].SetContravariant(true);
                clone[2].SetContravariant(true);

                return Epsilon(clone);
            }

            Tensor::Tensor Gamma(const Indices& indices) {
                return Tensor::Tensor::Gamma(indices);
            }

            Tensor::Tensor InverseGamma(const Indices& indices) {
                auto clone = indices;

                clone[0].SetContravariant(true);
                clone[1].SetContravariant(true);

                return Gamma(clone);
            }

            Tensor::Tensor EpsilonGamma(const Indices& indices) {
                // Calculate the numbers of epsilon and gammas
                unsigned N = indices.Size();
                unsigned numEpsilon = (N % 2 == 1) ? 1 : 0;
                unsigned numGamma   = (N % 2 == 1) ? (N-3)/2 : N/2;

                // Generate a epsilon gamma Tensor::Tensor
                return Tensor::Tensor::EpsilonGamma(numEpsilon, numGamma, indices);
            }

            Tensor::Tensor Delta(const Indices& indices) {
                return Tensor::Tensor::Delta(indices);
            }

            //#include <generator/coefficient.hpp

            Tensor::Tensor Coefficient(unsigned l, unsigned ld, unsigned r, unsigned rd) {
                // Get index blocks
                auto block1 = Construction::Tensor::Indices::GetRomanSeries(l, {1,3});
                auto block2 = Construction::Tensor::Indices::GetRomanSeries(ld, {1,3}, 15);
                auto block3 = Construction::Tensor::Indices::GetRomanSeries(r, {1,3}, l);
                auto block4 = Construction::Tensor::Indices::GetRomanSeries(rd, {1,3}, ld+15);

                auto indices = block1;
                indices.Append(block2);
                indices.Append(block3);
                indices.Append(block4);

                // Build indices for block symmetries
                auto block = block3;
                block.Append(block4);
                block.Append(block1);
                block.Append(block2);

                // Generate the tensor
                auto tensor = API::Arbitrary(indices);

                // Symmetrize the tensor
                if (l > 1) {
                    tensor = std::move(tensor.Symmetrize(block1));
                }

                if (ld > 1) {
                    tensor = std::move(tensor.Symmetrize(block2));
                }

                if (r > 1) {
                    tensor = std::move(tensor.Symmetrize(block3));
                }

                if (rd > 1) {
                    tensor = std::move(tensor.Symmetrize(block4));
                }

                // Exchange symmetrize if necessary
                if (l == r && ld == rd) {
                    tensor = std::move(tensor.ExchangeSymmetrize(indices, block));
                }

                // Collect terms
                tensor = std::move(tensor.Simplify().RedefineVariables("e"));

                return tensor;
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
                return tensor.Symmetrize(indices);
            }

            Tensor::Tensor AntiSymmetrize(const Tensor::Tensor& tensor, const Indices& indices) {
                return tensor.AntiSymmetrize(indices);
            }

            Tensor::Tensor ExchangeSymmetrize(const Tensor::Tensor& tensor, const Indices& from, const Indices& indices) {
                return tensor.ExchangeSymmetrize(from, indices);
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

            Tensor::Tensor RenameIndices(const Tensor::Tensor& tensor, const Indices& from, const Indices& to) {
                auto clone = tensor;

                if (from.Size() != to.Size()) return clone;

                // If the tensor is zero, return
                if (clone.IsZeroTensor()) return clone;
                if (clone.IsScalar()) return clone;

                std::map<Tensor::Index, Tensor::Index> mapping;
                for (unsigned i=0; i<from.Size(); ++i) {
                    mapping[from[i]] = to[i];
                }

                clone.SetIndices(clone.GetIndices().Shuffle(mapping));

                return clone.Canonicalize();
            }

            bool IsSymmetric(const Tensor::Tensor& tensor, const Indices& indices) {
                return tensor.Symmetrize(indices).IsEqual(tensor);
            }

            bool IsAntiSymmetric(const Tensor::Tensor& tensor, const Indices& indices) {
                return tensor.AntiSymmetrize(indices).IsEqual(tensor);
            }

            bool HasExchangeSymmetry(const Tensor::Tensor& tensor, const Indices& indices) {
                auto summands = tensor.GetSummands();

                for (auto& _tensor : summands) {
                    auto oldIndices = tensor.GetIndices();
                    auto permutation = Tensor::Permutation::From(indices, _tensor.GetIndices());
                    auto newIndices = permutation(oldIndices);

                    auto tensor = _tensor.SeparateScalefactor().second;

                    auto clone = tensor;
                    clone.SetIndices(newIndices);

                    auto subst = Tensor::Tensor::Substitute(clone, _tensor.GetIndices());

                    if (!subst.IsEqual(tensor)) return false;
                }

                return true;
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

            Substitution HomogeneousSystem(const Tensor::Tensor& tensor) {
                auto system = tensor.ToHomogeneousLinearSystem();

                Construction::Logger::Debug("Start reducing the equation ...");

                // Remove double lines
                {
                    std::vector<Construction::Vector::Vector<Construction::Tensor::Fraction>> vector;
                    for (int i = 0; i < system.first.GetNumberOfRows(); ++i) {
                        auto v = system.first.GetRowVector(i);

                        auto it = std::find(vector.begin(), vector.end(), v);
                        if (it == vector.end()) {
                            vector.push_back(v);
                            continue;
                        }

                        // Set all entries in the row to zero
                        for (int j = 0; j < system.first.GetNumberOfColumns(); ++j) {
                            system.first(i, j) = Tensor::Fraction(0,1);
                        }
                    }
                }

                Construction::Logger::Debug("Matrix is ", system.first.ToString(false));

                // Reduce
                system.first.ToRowEchelonForm();

                Construction::Logger::Debug("Matrix is ", system.first.ToString(false));

                Construction::Logger::Debug("Finished Gaussian elimination.");

                Substitution result;

                // Extract the results
                for (int i=0; i<system.first.GetNumberOfRows(); i++) {
                    auto vec = system.first.GetRowVector(i);

                    Construction::Logger::Debug("Row = ", vec);

                    // If the vector has zero norm, we get no further information => quit
                    if (vec.IsZero()) break;

                    bool isZero = true;
                    Scalar lhs = Scalar::Fraction(0,1);
                    Scalar rhs = Scalar::Fraction(0,1);

                    // Iterate over all the components
                    for (int j=0; j<vec.GetDimension(); j++) {
                        //Construction::Logger::Debug("j = ", j, ", vec[j] = ", vec[j], ", isZero = ", (isZero) ? "yes" : "no");

                        if (vec[j] == Construction::Tensor::Fraction(0,1) && isZero) continue;
                        if (vec[j] == Construction::Tensor::Fraction(1,1) && isZero) {
                            lhs = system.second[j];
                            isZero = false;
                        } else if (vec[j] != Construction::Tensor::Fraction(0,1)) {
                            rhs += (-system.second[j] * Tensor::Scalar::Fraction(vec[j].GetNumerator(), vec[j].GetDenominator()));
                        }
                    }

                    Construction::Logger::Debug("Found ", lhs, " = ", rhs);

                    // If the left hand side is not a variable, throw exception
                    if (lhs.IsNumeric() && lhs.ToDouble() == 0) {
                        throw Tensor::InvalidSubstitutionException();
                    }

                    // Add to the result
                    result.Insert(lhs, rhs);
                }

                Construction::Logger::Debug("Created substitution");

                return result;
            }

            Tensor::Tensor Substitute(const Tensor::Tensor& tensor, const Substitution& substitution) {
                return substitution(tensor);
            }

            Scalar Evaluate(const Tensor::Tensor& tensor, const std::vector<unsigned>& indices) {
                return tensor(indices);
            }

        }

    }
}
