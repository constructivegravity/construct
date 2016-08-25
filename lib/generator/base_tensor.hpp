#pragma once

#include <cassert>

#include <common/time_measurement.hpp>

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/tensor_container.hpp>

using Construction::Tensor::Indices;
using Construction::Tensor::Tensor;
using Construction::Tensor::TensorContainer;
using Construction::Tensor::TensorPointer;
using Construction::Tensor::EpsilonTensor;
using Construction::Tensor::MultipliedTensor;
using Construction::Tensor::GammaTensor;

template<typename T, typename S>
std::ostream& operator<<(std::ostream& os, const std::pair<S,T>& vec) {
    os << "(" << vec.first << ", " << vec.second << ")";
    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    os << "[";
    for (int i=0; i<vec.size(); i++) {
        os << vec[i];
        if (i != vec.size()-1) os << ", ";
    }
    os << "]";
    return os;
}

namespace Construction {
    namespace Generator {

        class BaseTensorGenerator {
        public:
            /**
                Generates the
             */
            std::vector<std::pair<Indices, Indices>> GenerateTwoPartition(const Indices& indices) const {
                assert(indices.Size() > 1);

                std::vector<std::pair<Indices, Indices>> result;

                // Take the first letter
                auto first = indices[0];

                for (int i=1; i<indices.Size(); i++) {
                    auto current = indices[i];

                    Indices remaining = indices;

                    Indices gamma;
                    gamma.Insert(first);
                    gamma.Insert(current);

                    remaining.Remove(i);
                    remaining.Remove(0);

                    result.push_back({ gamma, remaining });
                }

                return result;
            }

            std::vector<Indices> GenerateEvenRank(const Indices& indices) const {
                auto res = GenerateTwoPartition(indices);
                std::vector<Indices> result;

                if (indices.Size() == 2) {

                    for (auto& pair : res) {
                        result.push_back(pair.first);
                    }
                } else {
                    for (auto& pair : res) {
                        auto newComb = GenerateEvenRank(pair.second);
                        for (auto &v : newComb) {
                            auto current = pair.first;
                            current.Append(v);
                            result.push_back(current);
                        }
                    }
                }

                return result;
            }

            std::vector<Indices> GenerateOddRank(const Indices& indices) const {
                // Get all the three partitions of indices
                auto partitions = indices.GetAllPartitions(3);

                std::vector<Indices> result;

                // Iterate over all partitions
                for (auto& partition : partitions) {
                    if (partition.second.Size() != 0) {
                        // Generate the two-partitions of the rest with GenerateEvenRank
                        auto list = GenerateEvenRank(partition.second);

                        // Iterate over all the possible gamma indices
                        for (auto &l : list) {
                            auto current = partition.first;
                            current.Append(l);
                            result.push_back(current);
                        }
                    } else {
                        result.push_back(partition.first);
                    }
                }

                return result;
            }
        public:
            /**
                \brief Generate all possible canonical tensors of rank n
             */
            Tensor::Tensor Generate(unsigned order) const {
                // Stuff
                assert(order > 1);

                // Generate indices
                auto indices = Indices::GetRomanSeries(order, {1,3}, 0);

                return Generate(indices);
            }

            Tensor::Tensor Generate(const Indices& indices) const {
                // Expect all the ranges to be in space
                unsigned order = indices.Size();
                for (auto& index : indices) {
                    if (index.GetRange() != Common::Range(1,3)) {
                        // throw error
                    }
                }

                Tensor::Tensor result = Tensor::Tensor::Zero();
                unsigned variableCounter = 0;

                // Calculate
                unsigned numEpsilon = (indices.Size() % 2 == 0) ? 0 : 1;
                unsigned numGammas  = (indices.Size() % 2 == 0) ? indices.Size()/2  : (indices.Size()-3)/2;

                std::vector<Indices> possibleIndices;

                if (numEpsilon == 1) {
                    possibleIndices = GenerateOddRank(indices);
                } else {
                    possibleIndices = GenerateEvenRank(indices);
                }

                std::vector<Tensor::Tensor> tensors;

                for (auto& newIndices : possibleIndices) {
                    // Create variable
                    Tensor::Scalar variable ("e", ++variableCounter);

                    tensors.push_back(variable * Tensor::Tensor::EpsilonGamma(numEpsilon, numGammas, newIndices));
                }

                return Tensor::Tensor::Add(tensors);
            }
        };

    }
}
