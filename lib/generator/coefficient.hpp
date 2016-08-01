#pragma once

#include <tensor/tensor_container.hpp>
#include <tensor/tensor_database.hpp>
#include <language/api.hpp>

using Construction::Tensor::TensorContainer;
using Construction::Tensor::TensorDatabase;

namespace Construction {
    namespace Generator {

        class CoefficientGenerator {
        public:
            CoefficientGenerator(std::shared_ptr<TensorDatabase> database) : database(database) { }
        public:
            std::string ToCommand(unsigned l, unsigned ld, unsigned r, unsigned rd) const {
                auto indices = Construction::Tensor::Indices::GetRomanSeries(l+r+ld+rd, {1,3});

                std::string previous = "Tensor(" + indices.ToCommand() + ")";

                auto block1 = Construction::Tensor::Indices::GetRomanSeries(l, {1,3});
                auto block2 = Construction::Tensor::Indices::GetRomanSeries(r, {1,3}, l);
                auto block3 = Construction::Tensor::Indices::GetRomanSeries(ld, {1,3}, l+r);
                auto block4 = Construction::Tensor::Indices::GetRomanSeries(rd, {1,3}, l+r+ld);

                if (block1.Size() > 0)
                    previous = "Symmetrize(" + previous + "," + block1.ToCommand() + ")";
                if (block2.Size() > 0)
                    previous = "Symmetrize(" + previous + "," + block2.ToCommand() + ")";
                if (block3.Size() > 0)
                    previous = "Symmetrize(" + previous + "," + block3.ToCommand() + ")";
                if (block4.Size() > 0)
                    previous = "Symmetrize(" + previous + "," + block4.ToCommand() + ")";

                Indices newIndices = block2;
                newIndices.Append(block1);
                newIndices.Append(block4);
                newIndices.Append(block3);

                previous = "ExchangeSymmetrize(" + previous + "," + newIndices.ToCommand() + ")";
                previous = "LinearIndependent(" + previous + ")";

                return previous;
            }

            TensorContainer Generate(unsigned l, unsigned ld, unsigned r, unsigned rd) {
                // If all already calculated, do not calculate this again
                std::string cmd = ToCommand(l,ld, r, rd);
                if (database != nullptr && database->Contains(cmd)) {
                    return (*database)[cmd];
                }

                auto indices = Construction::Tensor::Indices::GetRomanSeries(l+r+ld+rd, {1,3});

                // If no indices, return scalar
                if (indices.Size() == 0) {
                    TensorContainer result;
                    result.Insert(std::make_shared<Construction::Tensor::ScalarTensor>(1));
                    return result;
                }

                // Generate the tensor
                std::string previous = "Tensor(" + indices.ToCommand() + ")";

                TensorContainer tensors;

                if (database != nullptr && database->Contains(previous)) {
                    std::unique_lock<std::mutex> lock(mutex);
                    tensors = (*database)[previous];
                } else {
                    tensors = Construction::Language::API::Tensor(indices);

                    std::unique_lock<std::mutex> lock(mutex);
                    (*database)[previous] = tensors;
                }

                // Symmetrize in the blocks
                auto block1 = Construction::Tensor::Indices::GetRomanSeries(l, {1,3});
                auto block2 = Construction::Tensor::Indices::GetRomanSeries(r, {1,3}, l);
                auto block3 = Construction::Tensor::Indices::GetRomanSeries(ld, {1,3}, l+r);
                auto block4 = Construction::Tensor::Indices::GetRomanSeries(rd, {1,3}, l+r+ld);

                if (block1.Size() > 0) {
                    previous = "Symmetrize(" + previous + "," + block1.ToCommand() + ")";

                    if (database != nullptr && database->Contains(previous)) {
                        std::unique_lock<std::mutex> lock(mutex);
                        tensors = (*database)[previous];
                    } else {
                        tensors = Construction::Language::API::Symmetrize(tensors, block1);

                        std::unique_lock<std::mutex> lock(mutex);
                        (*database)[previous] = tensors;
                    }
                }
                if (block2.Size() > 0) {
                    previous = "Symmetrize(" + previous + "," + block2.ToCommand() + ")";

                    if (database != nullptr && database->Contains(previous)) {
                        std::unique_lock<std::mutex> lock(mutex);
                        tensors = (*database)[previous];
                    } else {
                        tensors = Construction::Language::API::Symmetrize(tensors, block2);

                        std::unique_lock<std::mutex> lock(mutex);
                        (*database)[previous] = tensors;
                    }
                }
                if (block3.Size() > 0) {
                    previous = "Symmetrize(" + previous + "," + block3.ToCommand() + ")";

                    if (database != nullptr && database->Contains(previous)) {
                        std::unique_lock<std::mutex> lock(mutex);
                        tensors = (*database)[previous];
                    } else {
                        tensors = Construction::Language::API::Symmetrize(tensors, block3);

                        std::unique_lock<std::mutex> lock(mutex);
                        (*database)[previous] = tensors;
                    }
                }
                if (block4.Size() > 0) {
                    previous = "Symmetrize(" + previous + "," + block4.ToCommand() + ")";

                    if (database != nullptr && database->Contains(previous)) {
                        std::unique_lock<std::mutex> lock(mutex);
                        tensors = (*database)[previous];
                    } else {
                        tensors = Construction::Language::API::Symmetrize(tensors, block4);

                        std::unique_lock<std::mutex> lock(mutex);
                        (*database)[previous] = tensors;
                    }
                }

                // Implement the exchange symmetry
                Indices newIndices = block2;
                newIndices.Append(block1);
                newIndices.Append(block4);
                newIndices.Append(block3);

                previous = "ExchangeSymmetrize(" + previous + "," + newIndices.ToCommand() + ")";

                if (database != nullptr && database->Contains(previous)) {
                    std::unique_lock<std::mutex> lock(mutex);
                    tensors = (*database)[previous];
                } else {
                    tensors = Construction::Language::API::ExchangeSymmetrize(tensors, newIndices);

                    std::unique_lock<std::mutex> lock(mutex);
                    (*database)[previous] = tensors;
                }

                // Choose basis
                previous = "LinearIndependent(" + previous + ")";

                if (database != nullptr && database->Contains(previous)) {
                    tensors = (*database)[previous];
                } else {
                    tensors = Construction::Language::API::LinearIndependent(tensors);

                    std::unique_lock<std::mutex> lock(mutex);
                    (*database)[previous] = tensors;
                }

                return tensors;
            }
        private:
            std::mutex mutex;
            std::shared_ptr<TensorDatabase> database;
        };

    }
}
