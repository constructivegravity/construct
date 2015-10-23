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

        CLI_COMMAND(Symmetrize, true)

            std::string Help() const {
                return "Symmetrize(<Tensors>, <Indices>)";
            }

            TensorContainer Execute() const {
                return API::Symmetrize(GetTensors(0), GetIndices(1));
            }

        };

        REGISTER_COMMAND(Symmetrize);
        REGISTER_ARGUMENT(Symmetrize, 0, ArgumentType::TENSOR);
        REGISTER_ARGUMENT(Symmetrize, 1, ArgumentType::INDEX);

        CLI_COMMAND(AntiSymmetrize, true)

            std::string Help() const {
                return "AntiSymmetrize(<Tensors>, <Indices>)";
            }

            TensorContainer Execute() const {
                return API::AntiSymmetrize(GetTensors(0), GetIndices(1));
            }

        };

        REGISTER_COMMAND(AntiSymmetrize);
        REGISTER_ARGUMENT(AntiSymmetrize, 0, ArgumentType::TENSOR);
        REGISTER_ARGUMENT(AntiSymmetrize, 1, ArgumentType::INDEX);

    }
}