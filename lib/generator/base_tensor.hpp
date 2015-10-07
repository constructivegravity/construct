#pragma once

#include <cassert>

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>

using Albus::Tensor::Indices;
using Albus::Tensor::Tensor;
using Albus::Tensor::TensorPointer;
using Albus::Tensor::EpsilonTensor;
using Albus::Tensor::MultipliedTensor;
using Albus::Tensor::GammaTensor;

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

namespace Albus {
    namespace Generator {

        class BaseTensorGenerator {
        public:
            std::vector<TensorPointer> Generate(unsigned order) {
                // Only have tensors with order > 1
                assert(order > 1);

                // Generate indices
                auto indices = Indices::GetRomanSeries(order, {1,3}, 0);

                std::vector<TensorPointer> result;

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
                    TensorPointer T;
                    if (numEpsilons == 1) {
                        T = std::make_shared<EpsilonTensor>(EpsilonTensor(partition[0]));
                    } else {
                        T = std::make_shared<GammaTensor>(GammaTensor(partition[0]));
                    }

                    int max = (numEpsilons == 1) ? numGammas : numGammas - 1;
                    for (int i = 1; i <= max; i++) {
                        TensorPointer S = std::move(T);
                        T = std::make_shared<MultipliedTensor>(S, std::make_shared<GammaTensor>(GammaTensor(partition[i])));
                    }

                    result.emplace_back(std::move(T));
                }

                return result;
            }
        private:

        };

    }
}