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
            \class Coefficient

            std:string Help
         */
        CLI_COMMAND(Coefficient, true)
                std::string Help() const {
                    return "Coefficient(<Numeric>, <Numeric>, <Numeric>, <Numeric>)";
                }

                TensorContainer Execute() const {
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
        CLI_COMMAND(Add, true)
            std::string Help() const {
                return "Add(<Tensor>, <Tensor>)";
            }

            static bool Cachable() {
                return false;
            }

            TensorContainer Execute() const {
                auto t = GetTensors(0);
                if (t.Size() != 1) {
                    std::cout << "  \033[31m" << "Not for tensor lists" << "\033[0m" << std::endl;
                    return TensorContainer();
                }

                auto result = t.Get(0);

                for (auto i=1; i<Size(); i++) {
                    auto s = GetTensors(i);
                    if (s.Size() != 1) {
                        std::cout << "  \033[31m" << "Not for tensor lists" << "\033[0m" << std::endl;
                        return TensorContainer();
                    }

                    result = API::Add(result, s.Get(0));
                }

                TensorContainer res;
                res.Insert(std::move(result));
                return res;
            }
        };

        REGISTER_COMMAND(Add);
        REGISTER_ARGUMENT(Add, 0, ArgumentType::TENSOR);
        REGISTER_REPEATED_ARGUMENT(Add, 1, ArgumentType::TENSOR);

        /**
            \class AddCommand

            Scale a tensor
         */
        CLI_COMMAND(Scale, true)
            std::string Help() const {
                return "Scale(<Tensor>, <Numeric>)";
            }

            static bool Cachable() {
                return false;
            }

            TensorContainer Execute() const {
                auto t = GetTensors(0);
                if (t.Size() == 0) {
                    return TensorContainer();
                }

                if (t.Size() != 1) {
                    std::cout << "  \033[31m" << "Not for tensor lists" << "\033[0m" << std::endl;
                    return TensorContainer();
                }

                auto result = t.Get(0);

                result = API::Scale(result, GetNumeric(1));

                TensorContainer res;
                res.Insert(std::move(result));
                return res;
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

            static bool Cachable() {
                return false;
            }

            TensorContainer Execute() const {
                std::cout << "  \033[32m" << API::DegreesOfFreedom(GetTensors(0)) << "\033[0m" << std::endl;
                return TensorContainer();
            }

        };

        REGISTER_COMMAND(DegreesOfFreedom);
        REGISTER_ARGUMENT(DegreesOfFreedom, 0, ArgumentType::TENSOR);

        CLI_COMMAND(IsZero, false)

            std::string Help() const {
                return "IsZero(<Tensor>)";
            }

            static bool Cachable() {
                return false;
            }

            TensorContainer Execute() const {
                auto t = GetTensors(0);
                if (t.Size() == 0) {
                    std::cout << "  \033[32m" << "Yes" << "\033[0m" << std::endl;
                    return TensorContainer();
                }

                if (t.Size() != 1) {
                    std::cout << "  \033[31m" << "Not for tensor lists" << "\033[0m" << std::endl;
                    return TensorContainer();
                }

                auto result = t.Get(0);
                if (result->IsZero()) {
                    std::cout << "  \033[32m" << "Yes" << "\033[0m" << std::endl;
                } else {
                    std::cout << "  \033[32m" << "No" << "\033[0m" << std::endl;
                }

                return TensorContainer();
            }

        };

        REGISTER_COMMAND(IsZero);
        REGISTER_ARGUMENT(IsZero, 0, ArgumentType::TENSOR);

        /**
            \class Evaluate

            std:string Help
         */
        CLI_COMMAND(Evaluate, false)
                std::string Help() const {
                    return "Evaluate(<Tensors>, <Numeric>...)";
                }

                TensorContainer Execute() const {
                    std::vector<unsigned> indices;

                    for (auto i=1; i<Size(); i++) {
                        unsigned j = static_cast<unsigned>(GetNumeric(i));
                        indices.push_back(j);
                    }

                    // Evaluate
                    auto result = API::Evaluate(GetTensors(0), indices);

                    // Print result
                    std::cout << "\033[32m";
                    for (int i=0; i<result.size(); i++) {
                        if (result[i] != 0) {
                            std::cout << "   " << result[i] << " * " << "e_" << (i+1) << " ";
                            if (i != result.size()-1) std::cout << "+ ";
                            std::cout << std::endl;
                        }
                    }
                    std::cout << "\033[0m";

                    return TensorContainer();
                }
        };

        REGISTER_COMMAND(Evaluate);
        REGISTER_ARGUMENT(Evaluate, 0, ArgumentType::TENSOR);
        REGISTER_REPEATED_ARGUMENT(Evaluate, 1, ArgumentType::NUMERIC);

    }
}
