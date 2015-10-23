#pragma once

#include <language/command.hpp>
#include <language/argument.hpp>
#include <language/api.hpp>

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>

#include <generator/base_tensor.hpp>

using Construction::Tensor::Tensor;

namespace Construction {
    namespace Language {

        /**
            \class TensorCommand

            Command that generates tensors for given indices out of
            epsilons and gammas. It uses the API function.
         */
        CLI_COMMAND(Tensor, true)
            std::string Help() const {
                return "Tensor(<Indices>)";
            }

            TensorContainer Execute() const {
                auto indices = GetArgument<IndexArgument>(0)->GetIndices();
                return API::Tensor(indices);
            }
        };

        REGISTER_COMMAND(Tensor);
        REGISTER_ARGUMENT(Tensor, 0, ArgumentType::INDEX);

        /**
            \class EpsilonGammaCommand

            Command that generates one tensor for given indices out of
            epsilons and gammas. It uses the API function.
         */
        CLI_COMMAND(EpsilonGamma, true)
            std::string Help() const {
                return "EpsilonGamma(<Indices>)";
            }

            TensorContainer Execute() const {
                return API::EpsilonGamma(GetIndices(0));
            }
        };

        REGISTER_COMMAND(EpsilonGamma);
        REGISTER_ARGUMENT(EpsilonGamma, 0, ArgumentType::INDEX);

        /**
            \class AppendCommand

            Command to append a tensor list to another one. Uses the
            API method internally.
         */
        CLI_COMMAND(Append, true)
            std::string Help() const {
                return "Append(<Tensors>, <Tensors>)";
            }

            TensorContainer Execute() const {
                return API::Append(GetTensors(0), GetTensors(1));
            }
        };

        REGISTER_COMMAND(Append);
        REGISTER_ARGUMENT(Append, 0, ArgumentType::TENSOR);
        REGISTER_ARGUMENT(Append, 1, ArgumentType::TENSOR);

        /**
            \class DegreesOfFreedomCommand

            Calculates the degrees of freedom of tensors. Uses the
            API command.
         */
        CLI_COMMAND(DegreesOfFreedom, false)

            std::string Help() const {
                return "DegreesOfFreedom(<Tensors>)";
            }

            TensorContainer Execute() const {
                std::cout << "  \033[32m" << API::DegreesOfFreedom(GetTensors(0)) << "\033[0m" << std::endl;
                return TensorContainer();
            }

        };

        REGISTER_COMMAND(DegreesOfFreedom);
        REGISTER_ARGUMENT(DegreesOfFreedom, 0, ArgumentType::TENSOR);

    }
}