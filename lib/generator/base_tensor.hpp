#pragma once

#include <cassert>

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
            TensorContainer Generate(unsigned order) {
                // Stuff
                assert(order > 1);

                // Generate indices
                auto indices = Indices::GetRomanSeries(order, {1,3}, 0);

                return Generate(indices);
            }

            TensorContainer Generate(const Indices& indices) {
                unsigned order = indices.Size();
                for (auto& index : indices) {
                    if (index.GetRange() != Common::Range(1,3)) {
                        // throw error
                    }
                }

                TensorContainer result;

                // Generate partitions
                unsigned numEpsilons = 0;
                unsigned numGammas   = 0;
                std::vector<std::vector<Indices>> partitions;
                {
                    std::vector<unsigned> vec;
                    if (order % 2 != 0) {
                        vec.push_back(3);
                        order -= 3;
                        numEpsilons = 1;
                    }
                    while (order > 0) {
                        vec.push_back(2);
                        order -= 2;
                        numGammas++;
                    }

                    // Get partitions
                    partitions = indices.GetAllPartitions(vec, true);
                }

                // Generate all the tensors with the given partitions
                for (auto& partition : partitions) {

                    // Join all indices in the partition
                    Tensor::Indices indices;

                    for (auto& p : partition) {
                        indices.Append(p);
                    }

                    result.Insert(std::make_shared<Tensor::EpsilonGammaTensor>(numEpsilons, numGammas, indices));

                    /*Tensor::Tensor T;
                    //TensorPointer T;
                    if (numEpsilons == 1) {
                        T = EpsilonTensor(partition[0]);
                        //T = std::make_shared<EpsilonTensor>(EpsilonTensor(partition[0]));
                    } else {
                        T = GammaTensor(partition[0]);
                        //T = std::make_shared<GammaTensor>(GammaTensor(partition[0]));
                    }

                    int max = (numEpsilons == 1) ? numGammas : numGammas - 1;
                    for (int i = 1; i <= max; i++) {
                        Tensor::Tensor S = T;
                        T = MultipliedTensor(std::make_shared<Tensor::Tensor>(S), std::make_shared<GammaTensor>(GammaTensor(partition[i])));
                        //T = std::make_shared<MultipliedTensor>(S, std::make_shared<GammaTensor>(GammaTensor(partition[i])));
                    }

                    result.Insert(std::move(T));*/
                }

                return result;
            }
        private:

        };

    }
}