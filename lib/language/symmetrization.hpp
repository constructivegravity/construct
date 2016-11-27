#pragma once

#include <language/command.hpp>
#include <language/argument.hpp>
#include <language/api.hpp>

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>

#include <generator/base_tensor.hpp>

using Construction::Tensor::Tensor;
using Construction::Tensor::Expression;

namespace Construction {
    namespace Language {

        CLI_COMMAND(Symmetrize)

            std::string Help() const {
                return "Symmetrize(<Tensors>, <Indices>, ...)";
            }

            static bool Cachable() {
                return false;
            }

            std::string ToLaTeX(const std::vector<std::string>& args) const {
                std::string output = "Symmetrize(";

                for (int i=0; i<args.size()-1; ++i) {
                    output = output + args[i] + ", ";
                }

                return output + args[args.size()-1] + ")";
            }

            Expression Execute() const {
                auto result = GetTensors(0);

                for (int i=1; i<Size(); i++) {
                    result = API::Symmetrize(result, GetIndices(i));
                }

                return result;
            }

        };

        REGISTER_COMMAND(Symmetrize);
        REGISTER_ARGUMENT(Symmetrize, 0, ArgumentType::TENSOR);
        REGISTER_REPEATED_ARGUMENT(Symmetrize, 1, ArgumentType::INDEX);

        CLI_COMMAND(AntiSymmetrize)

            std::string Help() const {
                return "AntiSymmetrize(<Tensors>, <Indices>, ...)";
            }

            static bool Cachable() {
                return false;
            }

            std::string ToLaTeX(const std::vector<std::string>& args) const {
                std::string output = "AntiSymmetrize(";

                for (int i=0; i<args.size()-1; ++i) {
                    output = output + args[i] + ", ";
                }

                return output + args[args.size()-1] + ")";
            }

            Expression Execute() const {
                auto result = GetTensors(0);

                for (int i=1; i<Size(); i++) {
                    result = API::AntiSymmetrize(result, GetIndices(i));
                }

                return result;
            }

        };

        REGISTER_COMMAND(AntiSymmetrize);
        REGISTER_ARGUMENT(AntiSymmetrize, 0, ArgumentType::TENSOR);
        REGISTER_REPEATED_ARGUMENT(AntiSymmetrize, 1, ArgumentType::INDEX);

        CLI_COMMAND(ExchangeSymmetrize)

            std::string Help() const {
                return "ExchangeSymmetrize(<Tensors>, <Indices>)";
            }

            static bool Cachable() {
                return true;
            }

            std::string ToLaTeX(const std::vector<std::string>& args) const {
                std::string output = "ExchangeSymmetrize(";

                for (int i=0; i<args.size()-1; ++i) {
                    output = output + args[i] + ", ";
                }

                return output + args[args.size()-1] + ")";
            }

            Expression Execute() const {
                return API::ExchangeSymmetrize(GetTensors(0), GetIndices(1), GetIndices(2));
            }

        };

        REGISTER_COMMAND(ExchangeSymmetrize);
        REGISTER_ARGUMENT(ExchangeSymmetrize, 0, ArgumentType::TENSOR);
        REGISTER_ARGUMENT(ExchangeSymmetrize, 1, ArgumentType::INDEX);
        REGISTER_ARGUMENT(ExchangeSymmetrize, 2, ArgumentType::INDEX);

        CLI_COMMAND(BlockSymmetrize)

            std::string Help() const {
                return "BlockSymmetrize(<Tensors>, <Indices>, ...)";
            };

            static bool Cachable() {
                return true;
            }

            std::string ToLaTeX(const std::vector<std::string>& args) const {
                std::string output = "BlockSymmetrize(";

                for (int i=0; i<args.size()-1; ++i) {
                    output = output + args[i] + ", ";
                }

                return output + args[args.size()-1] + ")";
            }

            Expression Execute() const {
                std::vector<Indices> indices;
                for (int i=1; i<Size(); i++) {
                    indices.push_back(GetIndices(i));
                }
                return API::BlockSymmetrize(GetTensors(0), indices);
            }

        };

        REGISTER_COMMAND(BlockSymmetrize);
        REGISTER_ARGUMENT(BlockSymmetrize, 0, ArgumentType::TENSOR);
        REGISTER_REPEATED_ARGUMENT(BlockSymmetrize, 1, ArgumentType::INDEX);


        CLI_COMMAND(IsSymmetric)

            std::string Help() const {
                return "IsSymmetric(<Tensors>, <Indices>)";
            }

            static bool Cachable() {
                return false;
            }

            Expression Execute() const {
                auto tensor = GetTensors(0);
                auto indices = GetIndices(1);

                return Expression::Boolean(API::IsSymmetric(tensor, indices));
            }

        };

        REGISTER_COMMAND(IsSymmetric);
        REGISTER_ARGUMENT(IsSymmetric, 0, ArgumentType::TENSOR);
        REGISTER_ARGUMENT(IsSymmetric, 1, ArgumentType::INDEX);

        CLI_COMMAND(HasExchangeSymmetry)

            std::string Help() const {
                return "HasExchangeSymmetry(<Tensor>, <Indices>)";
            }

            static bool Cachable() {
                return false;
            }

            Expression Execute() const {
                return Expression::Boolean(API::HasExchangeSymmetry(GetTensors(0), GetIndices(1)));
            }

        };

        REGISTER_COMMAND(HasExchangeSymmetry);
        REGISTER_ARGUMENT(HasExchangeSymmetry, 0, ArgumentType::TENSOR);
        REGISTER_ARGUMENT(HasExchangeSymmetry, 1, ArgumentType::INDEX);

    }
}
