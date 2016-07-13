#pragma once

#include <common/task_pool.hpp>
#include <common/time_measurement.hpp>

#include <tensor/tensor.hpp>
#include <tensor/symmetrization.hpp>

using Construction::Tensor::Symmetrization;
using Construction::Tensor::AntiSymmetrization;
using Construction::Tensor::BlockSymmetrization;

namespace Construction {
    namespace Generator {

        using Tensor::Indices;

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
            Symmetry(const std::vector<Tensor::Indices>& blocks) : blocks(blocks) {
                if (blocks.size() > 1) {
                    type = SymmetryType::BLOCKSYMMETRY;
                }
            }

            Symmetry(const Tensor::Indices& indices) {
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
            std::vector<Tensor::Indices> blocks;
        };

        class SymmetrizedTensorGenerator {
        public:
            SymmetrizedTensorGenerator(const Indices& symmetrization) : symmetrization(symmetrization) { }
        public:
            Tensor::Tensor operator()(const Tensor::Tensor& tensor, bool scaledResult=true) const {
                Tensor::Tensor result = Tensor::Tensor::Zero();

                // Extract summands
                std::vector<Tensor::Tensor> _tensors = tensor.GetSummands();

                // Task pool
                Common::TaskPool pool;

                // Helper map
                std::mutex mutex;
                std::map<unsigned, Tensor::Tensor> tensors;

                // Helper function
                auto helper = [&](const Tensor::Tensor& _tensor, unsigned id) {
                    // Separate the scale factor, if present
                    auto scale = _tensor.SeparateScalefactor().first;
                    auto tensor = _tensor.SeparateScalefactor().second;

                    // Build symmetrization
                    std::vector<unsigned> s;
                    for (auto& index : symmetrization) {
                        int pos = _tensor.GetIndices().IndexOf(index) + 1;
                        s.push_back(pos);
                    }

                    Symmetrization symm(s, scaledResult);

                    // Symmetrize
                    auto newTensor = scale * symm(tensor);

                    // Check if the tensor is zero
                    if (!newTensor.IsZero()) {
                        std::unique_lock<std::mutex> lock(mutex);
                        tensors.insert({ id, std::move(newTensor) });
                    }
                };

                // Symmetrize every tensor in the list
                for (unsigned i=0; i<_tensors.size(); ++i) {
                    pool.Enqueue(helper, _tensors[i], i);
                }

                // Wait for the tasks to finish
                pool.Wait();

                // Build the result
                for (auto& pair : tensors) {
                    result += pair.second;
                }

                return result;
            }
        private:
            Indices symmetrization;
        };

        class AntiSymmetrizedTensorGenerator {
        public:
            AntiSymmetrizedTensorGenerator(const Indices& symmetrization) : symmetrization(symmetrization) { }
        public:
            Tensor::Tensor operator()(const Tensor::Tensor& tensor, bool scaledResult=true) const {
                Tensor::Tensor result = Tensor::Tensor::Zero();

                // Extract summands
                std::vector<Tensor::Tensor> tensors = tensor.GetSummands();

                // Task pool
                Common::TaskPool pool (1);

                // Helper map
                std::map<unsigned, Tensor::Tensor> _tensors;

                // Helper function
                auto helper = [&](const Tensor::Tensor& _tensor, unsigned id) {
                    // Separate the scale factor, if present
                    auto scale = _tensor.SeparateScalefactor().first;
                    auto tensor = _tensor.SeparateScalefactor().second;

                    // Build symmetrization
                    std::vector<unsigned> s;
                    for (auto& index : symmetrization) {
                        int pos = _tensor.GetIndices().IndexOf(index) + 1;
                        s.push_back(pos);
                    }

                    AntiSymmetrization symm(s, scaledResult);

                    // Symmetrize
                    auto newTensor = scale * symm(tensor);

                    // Check if the tensor is zero
                    if (!newTensor.IsZero()) {
                        _tensors.insert({ id, newTensor });
                    }
                };

                // Symmetrize every tensor in the list
                for (unsigned i=0; i<tensors.size(); ++i) {
                    pool.Enqueue(helper, tensors[i], i);
                }

                // Wait for the tasks to finish
                pool.Wait();

                // Build the result
                for (auto& pair : _tensors) {
                    result += pair.second;
                }

                return result;
            }
        private:
            Indices symmetrization;
        };

        class ExchangeSymmetrizedTensorGenerator {
        public:
            ExchangeSymmetrizedTensorGenerator(const Indices& indices) : indices(indices) { }
        public:
            Tensor::Tensor operator()(const Tensor::Tensor& tensor, bool scaledResult=true) const {
                Tensor::Tensor result = Tensor::Tensor::Zero();

                auto tensors = tensor.GetSummands();

                for (auto& _tensor : tensors) {
                    auto oldIndices = tensor.GetIndices();
                    auto permutation = Tensor::Permutation::From(indices, _tensor.GetIndices());
                    auto newIndices = permutation(oldIndices);

                    auto scale = _tensor.SeparateScalefactor().first;
                    auto tensor = _tensor.SeparateScalefactor().second;

                    auto copyTensor = tensor;
                    copyTensor.SetIndices(newIndices);

                    auto added = tensor + copyTensor;
                    auto scaled = Tensor::Scalar(1,2) * added;

                    if (scaled.IsEqual(tensor)) {
                        result += _tensor;
                        continue;
                    }

                    if (!scaledResult)
                        result += scale * added;
                    else
                        result += scale * scaled;
                }

                return result;
            }
        private:
            Indices indices;
        };

        /*class BlockSymmetrizedTensorGenerator {
        public:
            BlockSymmetrizedTensorGenerator(const std::vector<Indices>& blocks) : blocks(blocks) { Validate(); }
        public:
            void Validate() const {
                // If no indices in there, symmetrization is trivial, therefore return true
                if (blocks.size() == 0) return;

                // Get the number of indices for the first element
                auto size = blocks[0].Size();

                // Iterate over all index blocks and check if the sizes match
                for (auto& indices : blocks) {
                    if (indices.Size() != size) {
                        throw Exception("Block symmetrization can only go over indices of same length");
                    }
                }
            }

            TensorContainer operator()(const TensorContainer& tensors, bool scaledResult=false) const {
                TensorContainer result;

                if (tensors.Size()== 0) return result;

                // Build symmetrization
                std::vector<std::pair<unsigned, unsigned>> s;

                for (auto& block : blocks) {
                    int first = tensors.Get(0)->GetIndices().IndexOf(block[0]) + 1;
                    int next = first + 1;
                    for (int j=1; j<block.Size(); j++) {
                        if (tensors.Get(0)->GetIndices().IndexOf(block[j])+1 != next) {
                            throw Exception("You need to specify blocks over neighbooring indices");
                        }
                        next++;
                    }
                    s.push_back({first, next-1});
                }
                BlockSymmetrization symm(s, scaledResult);

                // Symmetrize everything in the list
                for (auto& tensor : tensors) {
                    // Symmetrize
                    auto newTensor = std::move(symm(tensor));

                    // Check if the tensor is zero
                    if (!newTensor->IsZero()) {
                        result.Insert(newTensor);
                    }
                }

                EquivalentSelector selector;
                return selector(result);
            }
        private:
            std::vector<Indices> blocks;
        };*/

    }
}
