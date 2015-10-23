#pragma once

#include <tensor/tensor.hpp>

namespace Construction {
    namespace Tensor {



        class Symmetrization {
        public:
            Symmetrization(const std::vector<unsigned>& indices) : indices(indices) { }

            Symmetrization(const Symmetrization& other) : indices(other.indices) { }
            Symmetrization(Symmetrization&& other) : indices(std::move(other.indices)) { }
        public:
            /**
                \brief Calculate the index combinations by permutation


             */
            std::vector<Indices> PermuteIndices(const Indices& indices) const {
                // Generate all the index permutations between the numbers in indices
                std::vector<Indices> permutations;

                // Helper method
                std::function<void(unsigned, Indices, Indices)> fn;
                fn = [&](unsigned i, Indices used, Indices unused) {
                    // if all indices are used,
                    if (unused.Size() == 0) {
                        permutations.push_back(used);
                    } else {
                        // if the current index is not part of the symmetrized indices, just insert and go to the next
                        if (std::find(this->indices.begin(), this->indices.end(), i+1) == this->indices.end()) {
                            used.Insert(indices[i]);
                            unused.Remove(std::distance(unused.begin(), std::find(unused.begin(), unused.end(), indices[i])));
                            fn(i+1, used, unused);
                        } else {
                            // else, iterate over the indices to change
                            for (auto& k : this->indices) {
                                Indices newUnused = unused;
                                Indices newUsed = used;

                                auto it = std::find(newUnused.begin(), newUnused.end(), indices[k-1]);

                                // if the index is unused, add it and call recursion for next index
                                if (it != newUnused.end()) {
                                    int pos = std::distance(newUnused.begin(), it);
                                    newUnused.Remove(std::distance(newUnused.begin(), it));
                                    newUsed.Insert(indices[k - 1]);
                                    fn(i + 1, newUsed, newUnused);
                                }
                            }
                        }
                    }
                };
                fn(0, {}, indices);

                return permutations;
            }

            /**
                \brief Symmetrize the tensor and return a list of the resulting tensors


             */
            std::vector<TensorPointer> Symmetrize(const ConstTensorPointer& tensor) const {
                // Calculate all the required permutations of the indices
                auto permutations = PermuteIndices(tensor->GetIndices());

                // Construct the linear combination
                std::vector<TensorPointer> tensors;
                for (auto& permutation : permutations) {
                    auto clone = tensor->Clone();
                    clone->SetIndices(permutation);
                    tensors.push_back(std::move(clone));
                }

                return tensors;
            }
        public:
            virtual TensorPointer operator()(const Tensor& tensor) const {
                return (*this)(ConstTensorPointer(ConstTensorPointer(), &tensor));
            }

            virtual TensorPointer operator()(const ConstTensorPointer& tensor) const {
                auto tensors = Symmetrize(tensor);

                // Construct the tensor sum
                TensorPointer last = std::move(tensors[0]);
                for (int i=1; i<tensors.size(); i++) {
                    last = std::move(std::make_shared<AddedTensor>(std::move(last), std::move(tensors[i])));
                }

                //return std::move(last);

                if (!std::make_shared<ScaledTensor>(last, 1.0/tensors.size())->IsEqual(*tensor)) {
                    return last; //std::move(last);
                } else {
                    return tensor->Clone(); //std::move(tensor->Clone());
                }

                return std::move(std::make_shared<ScaledTensor>(std::move(last), 1.0/tensors.size()));
            }
        public:
            std::vector<unsigned>::iterator begin() { return indices.begin(); }
            std::vector<unsigned>::iterator end() { return indices.end(); }

            std::vector<unsigned>::const_iterator begin() const { return indices.begin(); }
            std::vector<unsigned>::const_iterator end() const { return indices.end(); }
        public:
            friend std::ostream& operator<<(std::ostream& os, const Symmetrization& sym) {
                os << "(";
                for (int i=0; i<sym.indices.size(); i++) {
                    os << sym.indices[i];
                    if (i != sym.indices.size()-1) os << ", ";
                }
                os << ")";
                return os;
            }
        private:
            std::vector<unsigned> indices;
        };

        class AntiSymmetrization : public Symmetrization {
        public:
            AntiSymmetrization(const std::vector<unsigned>& indices) : Symmetrization(indices) { }
        public:
            virtual TensorPointer operator()(const Tensor& tensor) const override {
                auto tensors = Symmetrize(ConstTensorPointer(ConstTensorPointer(), &tensor));

                // Construct the tensor sum
                TensorPointer last = std::move(tensors[0]);
                for (int i=1; i<tensors.size(); i++) {
                    // Find sign
                    int sign = Permutation::From(tensors[i]->GetIndices(), tensor.GetIndices()).Sign();

                    if (sign > 0) {
                        last = std::move(std::make_shared<AddedTensor>(std::move(last), std::move(tensors[i])));
                    } else {
                        TensorPointer current = std::move(tensors[i]);

                        // If the tensor is already scaled, simply rescale. Simplifies much especially printing
                        if (current->IsScaledTensor()) {
                            static_cast<ScaledTensor*>(current.get())->SetScale(-static_cast<ScaledTensor*>(current.get())->GetScale());
                        }
                        // scale the tensor
                        else {
                            current = std::make_shared<ScaledTensor>(current, -1);
                        }

                        last = std::move(std::make_shared<AddedTensor>(std::move(last), std::move(current)));
                    }
                }

                return std::move(last);
                return std::move(std::make_shared<ScaledTensor>(std::move(last), 1.0/tensors.size()));
            }

            virtual TensorPointer operator()(const ConstTensorPointer& tensor) const override {
                auto tensors = Symmetrize(tensor);

                // Construct the tensor sum
                TensorPointer last = std::move(tensors[0]);
                for (int i=1; i<tensors.size(); i++) {
                    // Find sign
                    int sign = Permutation::From(tensors[i]->GetIndices(), tensor->GetIndices()).Sign();

                    if (sign > 0) {
                        last = std::move(std::make_shared<AddedTensor>(std::move(last), std::move(tensors[i])));
                    } else {
                        TensorPointer current = std::move(tensors[i]);

                        // If the tensor is already scaled, simply rescale. Simplifies much especially printing
                        if (current->IsScaledTensor()) {
                            static_cast<ScaledTensor*>(current.get())->SetScale(-static_cast<ScaledTensor*>(current.get())->GetScale());
                        }
                            // scale the tensor
                        else {
                            current = std::make_shared<ScaledTensor>(current, -1);
                        }

                        last = std::move(std::make_shared<AddedTensor>(std::move(last), std::move(current)));
                    }
                }

                return std::move(last);
                return std::move(std::make_shared<ScaledTensor>(std::move(last), 1.0/tensors.size()));
            }
        };

    }
}