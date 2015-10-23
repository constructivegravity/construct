#pragma once

#include <tensor/tensor.hpp>
#include <tensor/tensor_container.hpp>
#include <tensor/symmetrization.hpp>

#include <generator/equivalent_selector.hpp>

using Construction::Tensor::Symmetrization;
using Construction::Tensor::AntiSymmetrization;

namespace Construction {
    namespace Generator {

        /**
            \class Symmetry


         */
        class Symmetry {
        public:
            enum class SymmetryType {
                SYMMETRY = 101,
                ANTISYMMETRY = 102,
                BLOCKSYMMETRY = 103
            };
        public:
            Symmetry(const std::vector<Indices>& blocks) : blocks(blocks) {
                if (blocks.size() > 1) {
                    type = SymmetryType::BLOCKSYMMETRY;
                }
            }

            Symmetry(const Indices& indices) {
                blocks.push_back(indices);
            }

            Symmetry(const Symmetry& other) : type(other.type), blocks(other.blocks) { }
            Symmetry(Symmetry&& other) : type(std::move(other.type)), blocks(std::move(other.blocks)) { }
        public:
            bool IsSymmetric() const { return type == SymmetryType::SYMMETRY; }
            bool IsAntiSymmetric() const { return type == SymmetryType::ANTISYMMETRY; }
            bool IsBlockSymmetric() const { return type == SymmetryType::BLOCKSYMMETRY; }
        private:
            SymmetryType type = SymmetryType::SYMMETRY;
            std::vector<Indices> blocks;
        };

        class SymmetrizedTensorGenerator {
        public:
            SymmetrizedTensorGenerator(const Indices& symmetrization) : symmetrization(symmetrization) { }
        public:
            TensorContainer operator()(const TensorContainer& tensors) const {
                TensorContainer result;

                // Symmetrize everything in the list
                for (auto& tensor : tensors) {
                    // Build symmetrization
                    std::vector<unsigned> s;
                    for (auto& index : symmetrization) {
                        int pos = tensor->GetIndices().IndexOf(index) + 1;
                        s.push_back(pos);
                    }
                    Symmetrization symm(s);

                    // Symmetrize
                    auto newTensor = std::move(symm(tensor));

                    // Check if the tensor is zero
                    if (!newTensor->IsZero()) {

                    result.Insert(newTensor);

                    //    result.Insert(newTensor->Clone());
                    //    result.Insert(std::move(newTensor));
                    }
                }

                EquivalentSelector selector;
                return selector(result);
            }
        private:
            Indices symmetrization;
        };

        class AntiSymmetrizedTensorGenerator {
        public:
            AntiSymmetrizedTensorGenerator(const Indices& symmetrization) : symmetrization(symmetrization) { }
        public:
            TensorContainer operator()(const TensorContainer& tensors) const {
                TensorContainer result;

                // Symmetrize everything in the list
                for (auto& tensor : tensors) {
                    // Build symmetrization
                    std::vector<unsigned> s;
                    for (auto& index : symmetrization) {
                        int pos = tensor->GetIndices().IndexOf(index) + 1;
                        s.push_back(pos);
                    }
                    AntiSymmetrization symm(s);

                    // Symmetrize
                    auto newTensor = symm(tensor);

                    // Check if the tensor is zero
                    if (!newTensor->IsZero()) result.Insert(std::move(newTensor));
                }

                EquivalentSelector selector;
                return selector(result);
            }
        private:
            Indices symmetrization;
        };

    }
}