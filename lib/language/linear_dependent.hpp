#pragma once

#include <language/command.hpp>
#include <language/argument.hpp>
#include <language/api.hpp>

#include <tensor/index.hpp>
#include <tensor/tensor.hpp>
#include <tensor/expression.hpp>

#include <generator/base_tensor.hpp>

using Construction::Tensor::Tensor;
using Construction::Tensor::Expression;

namespace Construction {
    namespace Language {

        /**
            \class TensorCommand

            Command that generates tensors for given indices out of
            epsilons and gammas. It uses the API function.
         */
        CLI_COMMAND(LinearIndependent)
            std::string Help() const {
                return "LinearIndependent(<Tensors>)";
            }

            Expression Execute() const {
                return GetTensors(0).Simplify().RedefineVariables("e");
            }
        };

        REGISTER_COMMAND(LinearIndependent);
        REGISTER_ARGUMENT(LinearIndependent, 0, ArgumentType::TENSOR);

    }
}