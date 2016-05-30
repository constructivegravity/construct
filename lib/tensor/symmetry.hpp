#pragma once

namespace Construction {
    namespace Tensor {



        /**
            \class SymmetryClosure

            For each index, we have a byte that indicates the group it belongs to.
            A symmetry operation is always indicated by 2


            (ab)c(de)f
                a  00010001
                b  00010010
                c  00000000
                d  00000100
                e  00101000
                f  00100000

                Groups:
                1  00000001
                2  00000001
                3  00000001
                4  00000001
                5  00000001
                6  00000001

                Symmetries:
         */
        class SymmetryClosure {
        public:
            enum SymmetryType {
                SYMMETRY,
                ANTISYMMETRY
            };
        public:
            SymmetryClosure(const Indices& indices) {

            }
        public:
            void Add(const std::vector<char>& possibility, bool sign) {
                possibilities.push_back({sign, possibility});
            }

            void Symmetrize(const std::vector<std::vector<char>>& groups) {

            }
        public:
            std::pair<bool, std::vector<char>> Get(const std::vector<char>& indexPositions) const {
                for (auto& v : possibilities) {
                    if (v.second == indexPositions) return v;
                }
                throw std::runtime_error("Did not find this element");
            }

            bool operator()(const std::vector<char>& indexPositions, bool* sign) const {
                std::pair<bool, std::vector<char>> pos;

                // Get position
                try {
                    pos = Get(indexPositions);
                } catch(...) {
                    return false;
                }

                // Get sign
                if (sign != nullptr) *sign = pos.first;

                return true;
            }
        private:
            std::vector<std::pair<bool, std::vector<char>>> possibilities;
        };


        /**
            \class ElementarySymmetry

            Used for symmetry deduction to get rid of the numerical evaluation
            as much as possible.

         */
        class ElementarySymmetry {
        public:
            enum class Type {
                SYMMETRY,
                ANTISYMMETRY,

                BLOCKSYMMETRY,
                ANTIBLOCKSYMMETRY
            };
        public:
            ElementarySymmetry(const std::vector<unsigned>& indices, bool symmetric=true) {
                type = (symmetric) ? Type::SYMMETRY : Type::ANTISYMMETRY;
                for (auto& i : indices) {
                    blocks.push_back({ i,i });
                }
            }

            ElementarySymmetry(const std::vector<std::pair<unsigned, unsigned>>& blocks, bool symmetric=true) {
                type = (symmetric) ? Type::BLOCKSYMMETRY : Type::ANTIBLOCKSYMMETRY;
                this->blocks = blocks;
            }
        public:
            bool IsEqual(const Indices& first, const Indices& second, bool ignoreSign=false) const {
                // If the sizes do not match, clearly false
                if (first.Size() != second.Size()) return false;
                if (first == second) return true;

                bool symmetric = (type == Type::SYMMETRY || type == Type::BLOCKSYMMETRY) ? true : false;

                Indices trimmedFirst;
                Indices trimmedSecond;

                // Trim
                for (auto& block : blocks) {
                    if (block.first >= first.Size() || block.second >= first.Size()) return false;

                    trimmedFirst.Insert(first[block.first]);
                    trimmedSecond.Insert(second[block.first]);
                }

                // If symmetric, check if the blocks are permutations of each other
                if (symmetric) {
                    return trimmedSecond.IsPermutationOf(trimmedFirst);
                }
                    // else check if the permutation is even
                else {
                    // if we ignore the sign, i.e. epsilon_abc ~~ epsilon_bac then also check
                    // if this simply is a permutation, else check for even permutation
                    if (ignoreSign) {
                        return trimmedSecond.IsPermutationOf(trimmedFirst);
                    } else return Permutation::From(trimmedFirst, trimmedSecond).IsEven();
                }
            }
        private:
            std::vector<std::pair<unsigned, unsigned>> blocks;
            Type type;
        };

        /**
            \class Symmetry
         */
        class Symmetry {
        public:
            void Add(const ElementarySymmetry& symmetry) {
                symmetries.push_back(symmetry);
            }

            bool IsEqual(const Indices& first, const Indices& second, bool ignoreSign=false) const {
                for (auto& symmetry : symmetries) {
                    if (!symmetry.IsEqual(first, second, ignoreSign)) return false;
                }
                return true;
            }
        private:
            std::vector<ElementarySymmetry> symmetries;
        };

    }
}