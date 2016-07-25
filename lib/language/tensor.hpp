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

        /**
            \class TensorCommand

            Command that generates tensors for given indices out of
            epsilons and gammas. It uses the API function.
         */
        CLI_COMMAND(Arbitrary)
            std::string Help() const {
                return "Arbitrary(<Indices>)";
            }

            Expression Execute() const {                
                auto indices = GetArgument<IndexArgument>(0)->GetIndices();
                return API::Arbitrary(indices);
            }
        };

        REGISTER_COMMAND(Arbitrary);
        REGISTER_ARGUMENT(Arbitrary, 0, ArgumentType::INDEX);

        /**
            \class EpsilonCommand

            Command that generates one tensor for given indices out of
            epsilons. It uses the API function.
         */
        CLI_COMMAND(Epsilon)
            std::string Help() const {
                return "Epsilon(<Indices>)";
            }

            Expression Execute() const {
                return API::Epsilon(GetIndices(0));
            }
        };

        REGISTER_COMMAND(Epsilon);
        REGISTER_ARGUMENT(Epsilon, 0, ArgumentType::INDEX);

        /**
            \class GammaCommand

            Command that generates one tensor for given indices out of
            epsilons. It uses the API function.
         */
        CLI_COMMAND(Gamma)
            std::string Help() const {
                return "Gamma(<Indices>)";
            }

            Expression Execute() const {
                return API::Gamma(GetIndices(0));
            }
        };

        REGISTER_COMMAND(Gamma);
        REGISTER_ARGUMENT(Gamma, 0, ArgumentType::INDEX);

        /**
            \class EpsilonGammaCommand

            Command that generates one tensor for given indices out of
            epsilons and gammas. It uses the API function.
         */
        CLI_COMMAND(EpsilonGamma)
            std::string Help() const {
                return "EpsilonGamma(<Indices>)";
            }

            Expression Execute() const {
                return API::EpsilonGamma(GetIndices(0));
            }
        };

        REGISTER_COMMAND(EpsilonGamma);
        REGISTER_ARGUMENT(EpsilonGamma, 0, ArgumentType::INDEX);

        /**
            \class DeltaCommand

            Command that generates one tensor for given indices out of
            epsilons. It uses the API function.
         */
        CLI_COMMAND(Delta)
            std::string Help() const {
                return "Delta(<Indices>)";
            }

            Expression Execute() const {
                return API::Delta(GetIndices(0));
            }
        };

        REGISTER_COMMAND(Delta);
        REGISTER_ARGUMENT(Delta, 0, ArgumentType::INDEX);

        /**
            \class Coefficient

            std:string Help
         */
        CLI_COMMAND(Coefficient)
                std::string Help() const {
                    return "Coefficient(<Numeric>, <Numeric>, <Numeric>, <Numeric>)";
                }

                Expression Execute() const {
                    return API::Coefficient(GetNumeric(0), GetNumeric(1), GetNumeric(2), GetNumeric(3));
                }
        };

        REGISTER_COMMAND(Coefficient);
        REGISTER_ARGUMENT(Coefficient, 0, ArgumentType::NUMERIC);
        REGISTER_ARGUMENT(Coefficient, 1, ArgumentType::NUMERIC);
        REGISTER_ARGUMENT(Coefficient, 2, ArgumentType::NUMERIC);
        REGISTER_ARGUMENT(Coefficient, 3, ArgumentType::NUMERIC);

        /**
            \class AddCommand

            Command to add two tensors
         */
        CLI_COMMAND(Add)
            std::string Help() const {
                return "Add(<Tensor>, <Tensor>...)";
            }

            static bool Cachable() {
                return false;
            }

            Expression Execute() const {
                auto t = GetTensors(0);
                
                auto result = Tensor::Tensor::Zero();

                for (auto i=0; i<Size(); i++) {
                    auto s = GetTensors(i);
                    result = API::Add(result, s);
                }

                return result;
            }
        };

        REGISTER_COMMAND(Add);
        REGISTER_ARGUMENT(Add, 0, ArgumentType::TENSOR);
        REGISTER_REPEATED_ARGUMENT(Add, 1, ArgumentType::TENSOR);

        /**
            \class AddCommand

            Scale a tensor
         */
        CLI_COMMAND(Scale)
            std::string Help() const {
                return "Scale(<Tensor>, <Numeric>)";
            }

            static bool Cachable() {
                return false;
            }

            Expression Execute() const {
                return API::Scale(GetTensors(0), GetNumeric(1));
            }
        };

        REGISTER_COMMAND(Scale);
        REGISTER_ARGUMENT(Scale, 0, ArgumentType::TENSOR);
        REGISTER_ARGUMENT(Scale, 1, ArgumentType::NUMERIC);

        /**
            \class AppendCommand

            Command to append a tensor list to another one. Uses the
            API method internally.
         */
        /*CLI_COMMAND(Append, true)
            std::string Help() const {
                return "Append(<Tensors>, <Tensors>)";
            }

            TensorContainer Execute() const {
                return API::Append(GetTensors(0), GetTensors(1));
            }
        };

        REGISTER_COMMAND(Append);
        REGISTER_ARGUMENT(Append, 0, ArgumentType::TENSOR);
        REGISTER_ARGUMENT(Append, 1, ArgumentType::TENSOR);*/

        /**
            \class DegreesOfFreedomCommand

            Calculates the degrees of freedom of tensors. Uses the
            API command.
         */
        CLI_COMMAND(DegreesOfFreedom)

            std::string Help() const {
                return "DegreesOfFreedom(<Tensors>)";
            }

            static bool Cachable() {
                return false;
            }

            Expression Execute() const {
                // TODO: 
                //std::cout << "  \033[32m" << API::DegreesOfFreedom(GetTensors(0)) << "\033[0m" << std::endl;
                return Tensor::Scalar(static_cast<int>(API::DegreesOfFreedom(GetTensors(0))));
            }

        };

        REGISTER_COMMAND(DegreesOfFreedom);
        REGISTER_ARGUMENT(DegreesOfFreedom, 0, ArgumentType::TENSOR);

        CLI_COMMAND(IsZero)

            std::string Help() const {
                return "IsZero(<Tensor>)";
            }

            static bool Cachable() {
                return false;
            }

            Expression Execute() const {
                return Expression::Boolean(GetTensors(0).IsZero());
            }

        };

        REGISTER_COMMAND(IsZero);
        REGISTER_ARGUMENT(IsZero, 0, ArgumentType::TENSOR);

        /**
            \class Evaluate

            std:string Help
         */
        CLI_COMMAND(Evaluate)
                std::string Help() const {
                    return "Evaluate(<Tensors>, <Numeric>...)";
                }

                Expression Execute() const {
                    std::vector<unsigned> indices;

                    for (auto i=1; i<Size(); i++) {
                        unsigned j = static_cast<unsigned>(GetNumeric(i));
                        indices.push_back(j);
                    }

                    // Evaluate
                    return API::Evaluate(GetTensors(0), indices);
                }
        };

        REGISTER_COMMAND(Evaluate);
        REGISTER_ARGUMENT(Evaluate, 0, ArgumentType::TENSOR);
        REGISTER_REPEATED_ARGUMENT(Evaluate, 1, ArgumentType::NUMERIC);

        /**
            \class Simplify

            std:string Help
         */        
        CLI_COMMAND(Simplify)
            std::string Help() const {
                return "Simplify(<Tensor>)";
            }

            Expression Execute() const {
                return GetTensors(0).Simplify();
            }
        };

        REGISTER_COMMAND(Simplify);
        REGISTER_ARGUMENT(Simplify, 0, ArgumentType::TENSOR);

        /**
            \class RedefineVariables

            std:string Help
         */        
        CLI_COMMAND(RedefineVariables)
            std::string Help() const {
                return "RedefineVariables(<Tensor>)";
            }

            Expression Execute() const {
                return GetTensors(0).RedefineVariables("e");
            }
        };

        REGISTER_COMMAND(RedefineVariables);
        REGISTER_ARGUMENT(RedefineVariables, 0, ArgumentType::TENSOR);

        /**
            \class HomogeneousSystem

            std:string Help
         */ 
        CLI_COMMAND(HomogeneousSystem)
            std::string Help() const {
                return "HomogeneousSystem(<Tensor>)";
            }

            Expression Execute() const {
                return API::HomogeneousSystem(GetTensors(0));
            }
        };

        REGISTER_COMMAND(HomogeneousSystem);
        REGISTER_ARGUMENT(HomogeneousSystem, 0, ArgumentType::TENSOR);

        /**
            \class Substitute

            std:string Help
         */ 
        CLI_COMMAND(Substitute)
            std::string Help() const {
                return "Substitute(<Tensor>, <Substitution>)";
            }

            Expression Execute() const {
                return API::Substitute(GetTensors(0), GetSubstitution(1));
            }
        };

        REGISTER_COMMAND(Substitute);
        REGISTER_ARGUMENT(Substitute, 0, ArgumentType::TENSOR);
        REGISTER_ARGUMENT(Substitute, 1, ArgumentType::SUBSTITUTION);

    }
}
