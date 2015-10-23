#pragma once

#include <stack>

#include <tensor/index.hpp>
#include <tensor/symmetrization.hpp>

namespace Construction {
    namespace Tensor {

        /**
            \class IndexParser

            \brief Class to parse a symmetrized index structure


         */
        class IndexParser {
        public:
            enum class IndexType {
                ROMAN = 101,
                GREEK = 102,

                CUSTOM = 201
            };

            enum class SymmetryType {
                BLOCK = 101,

                SYMMETRY = 201,
                ANTISYMMETRY = 202,

                CUSTOM = 301
            };
        public:
            IndexParser() = default;

            IndexParser(IndexType type) : indexType(type) { }
        public:
            /**
                \brief Parse the string
             */
            void Parse(const std::string& text) const {
                /*std::stack< std::pair<SymmetryType, unsigned> > stack;

                Indices indices;

                // Iterate over all indices
                for (int pos = 0; pos < text.length(); pos++) {
                    // Get current sign
                    char c = text[pos];
                    int start = pos;

                    switch (c) {
                        // Begin symmetrization
                        case '(':
                            stack.push({SymmetryType::SYMMETRY, c});
                            break;

                        case '[':
                            stack.push({SymmetryType::ANTISYMMETRY,c});
                            break;

                        case ')':
                            // If the last open symmetry was not (, throw error
                            if (stack.top().first != SymmetryType::SYMMETRY) {
                                // throw error
                            }
                            start = stack.top().second;
                            stack.pop();

                            // Add the symmetry

                            break;

                        case ']':
                            // If the last open symmetry was not (, throw error
                            if (stack.top().first != SymmetryType::ANTISYMMETRY) {
                                // throw error
                            }
                            start = stack.top().second;
                            stack.pop();


                            break;

                        // Ignore whitespaces
                        case ' ':
                            break;

                        default:
                            Index index (std::string(1, c), {1,3});
                            indices.Insert(index);
                            break;

                    }
                }

                std::cout << indices << std::endl;*/

            }
        private:
            IndexType indexType = IndexType::ROMAN;
        };

    }
}