#pragma once

#include <tensor/tensor.hpp>

namespace Construction {
    namespace Tensor {

        class Symmetrization {
        public:
            Symmetrization(bool scaledResult=true) : scaledResult(scaledResult) { }
            Symmetrization(const std::vector<unsigned>& indices, bool scaledResult=false) : indices(indices), scaledResult(scaledResult) { }

            Symmetrization(const Symmetrization& other) : indices(other.indices), scaledResult(other.scaledResult) { }
            Symmetrization(Symmetrization&& other) : indices(std::move(other.indices)), scaledResult(std::move(other.scaledResult)) { }
        public:
            /**
                \brief Calculate the index combinations by permutation


             */
            virtual std::vector<Indices> PermuteIndices(const Indices& indices) const {
                // Generate all the index permutations between the numbers in indices
                std::vector<Indices> permutations;

                // Task pool
                //Common::TaskPool pool;

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
                            //pool.Enqueue(fn, i+1, used, unused);
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
                                    //pool.Enqueue(fn, i+1, newUsed, newUnused);
                                }
                            }
                        }
                    }
                };

                // Let the magic happen ...
                Indices empty = {};
                //pool.Enqueue(fn, 0, empty, indices);
                fn(0, {}, indices);

                // Wait for everything to finish
                //pool.Wait();

                return permutations;
            }

            /**
                \brief Symmetrize the tensor and return a list of the resulting tensors


             */
            std::vector<Tensor> Symmetrize(const Tensor& tensor) const {
                // Calculate all the required permutations of the indices
                auto indices = tensor.GetIndices();
                auto permutations = PermuteIndices(indices);

                // Initialize result
                std::vector<Tensor> tensors;

                // Helper function
                for (auto permutation : permutations) {
                    Tensor clone = tensor;
                    clone.SetIndices(permutation);

                    tensors.push_back(clone);
                }

                return tensors;
            }
        public:
            virtual Tensor operator()(const Tensor& tensor) const {
                auto tensors = Symmetrize(tensor);

                // Add all the tensors
                Tensor last = Tensor::Zero();
                for (auto& t : tensors) {
                    last += t;
                }

                Tensor scaled = Scalar(1, tensors.size()) * last;

                // If the symmetrized tensor is different, scale
                /*if (!scaled.IsEqual(tensor)) {
                    if (scaledResult) return scaled;
                    else return last;
                } else return tensor;*/

                return scaled;
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
        protected:
            bool scaledResult = true;
        };

        class AntiSymmetrization : public Symmetrization {
        public:
            AntiSymmetrization(const std::vector<unsigned>& indices, bool scaledResult=false) : Symmetrization(indices, scaledResult) { }
        public:
            virtual Tensor operator()(const Tensor& tensor) const override {
                auto tensors = Symmetrize(tensor);

                // Construct the tensor sum
                Tensor last = std::move(tensors[0]);
                for (int i=1; i<tensors.size(); i++) {
                    auto _indices = (tensors[i].IsSubstitute()) ? tensors[i].As<SubstituteTensor>()->GetPermutedIndices() : tensors[i].GetIndices();

                    // Find sign
                    int sign = Permutation::From(_indices, tensor.GetIndices()).Sign();

                    // Either add or subtract, depending on sign
                    if (sign > 0) {
                        last = std::move(last + tensors[i]);
                    } else {
                        last = std::move(last - tensors[i]);
                    }
                }

                Tensor scaled = Scalar(1, tensors.size()) * last;
                if (!scaled.IsEqual(tensor)) {
                    if (scaledResult)
                        return scaled;
                    else return last;
                } else {
                    return tensor;
                }
            }
        };

        class BlockSymmetrization : public Symmetrization {
        public:
            BlockSymmetrization(const std::vector<std::pair<unsigned, unsigned>>& blocks, bool scaledResult=false) : blocks(blocks), Symmetrization(scaledResult) { }
        public:
            /**
                \brief Calculate the index combinations by permutation


             */
            virtual std::vector<Indices> PermuteIndices(const Indices& indices) const override {
                // Create the shortened indices, each block exchanged with a multi-index
                Indices shortened = indices;
                std::vector<unsigned> toPermute;
                std::map<Index, Indices> groups;

                for (int i=0; i<blocks.size(); i++) {
                    if (blocks[i].second < blocks[i].first) continue;

                    auto index = indices[blocks[i].first-1];
                    groups[index].Insert(index);

                    // Add index to permute
                    for (int j=blocks[i].first+1; j<=blocks[i].second; j++) {
                        auto next = indices[j-1];
                        groups[index].Insert(next);

                        int id = shortened.IndexOf(next);
                        shortened.Remove(id);
                    }
                    toPermute.push_back(shortened.IndexOf(index)+1);
                }

                // Create permutation of the blocks
                Symmetrization sym(toPermute);
                std::vector<Indices> temporaryPermutations = sym.PermuteIndices(shortened);

                // Get back the original terms
                std::vector<Indices> permutations;
                for (auto& permutation : temporaryPermutations) {
                    Indices newIndices;

                    for (auto& index : permutation) {
                        auto it = groups.find(index);

                        // If the index is not part of the multi-indices, push back the index
                        if (it == groups.end()) {
                            newIndices.Insert(index);
                            continue;
                        }

                        newIndices.Append(it->second);
                    }

                    permutations.push_back(newIndices);
                }

                return permutations;
            }
        private:
            std::vector< std::pair<unsigned, unsigned> > blocks;
        };


    }
}
