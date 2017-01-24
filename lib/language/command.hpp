#pragma once

#include <map>
#include <iostream>

#include <common/singleton.hpp>
#include <common/error.hpp>
#include <language/argument.hpp>

//#include <tensor/tensor_container.hpp>
#include <tensor/expression.hpp>
#include <tensor/tensor.hpp>
#include <tensor/scalar.hpp>

using Construction::Tensor::Expression;

namespace Construction {

    class WrongNumberOfArgumentsException : public Exception {
    public:
        WrongNumberOfArgumentsException() : Exception("The command was called with the wrong number of arguments") { }
    };

    class WrongArgumentTypeException : public Exception {
    public:
        WrongArgumentTypeException() : Exception("The command was called with a wrong argument type") { }
        WrongArgumentTypeException(const std::string& expected, const std::string& got) : expected(expected), got(got), Exception("The command was called with a wrong argument type") { }
    public:
        std::string expected;
        std::string got;
    };

    class UnknownCommandException : public Exception {
    public:
        UnknownCommandException() : Exception("The command is unknown") { }
    };

    namespace Language {

        /**
            \class Command

            In order to implement all the different commands for the
            language, one can derive from this base class and register
            in the global command factory. By this we automatically
            insert an entry into the CLI class that allows the system
            to understand the call.

            By itself, Command is an abstract class with two virtual
            methods: `Help` and `Execute`. Both have to be overwritten
            in order to obtain a proper command.

            To simplify the declaration of a call, we defined three
            different macros: `CLI_COMMAND`, `REGISTER_COMMAND` and
            `REGISTER_ARGUMENT`.

            The first macro creates the class for the command and
            sets up the properties. Its arguments are the name of the
            command (NOT AS STRING!) and a boolean to denote if the
            command returns a list of tensors.

            The second is essential to register the command at the
            CLI. If one forgets the registration the command will not
            be understand by the system.

            The third macro allows the registration of the arguments of
            the command. This allows an automatic validation of the input
            before the execute method of the command is called. As a result
            it is never necessary to validate the arguments. Pretty hand,
            isn't it?

            Example:
                CLI_COMMAND(Test) {
                    std::string Help() const {
                        return "Test(<Indices>)";
                    }

                    TensorContainer Execute() const {
                        auto indices = GetArgument<IndexArgument>(0)->GetIndices();
                        std::cout << indices << std::endl;
                        return TensorContainer();
                    }
                };

                REGISTER_COMMAND(Test);
                REGISTER_ARGUMENT(Test, 0, ArgumentType::INDEX);
         */
        class Command {
        public:
            Command() = default;
            Command(const std::string& name) : name(name) { }
        public:
            std::string GetName() const { return name; }
            void SetName(const std::string& name) { this->name = name; }
        public:
            void AddArgument(const std::shared_ptr<BaseArgument>& arg) {
                arguments.push_back(arg);
            }

            size_t Size() const { return arguments.size(); }
        public:
            /**
                \brief Gets a casted pointer to the argument of specified type

                This casts the argument to the desired type, i.e. IndexArgument
                or TensorArgument and returns the pointer. It simplifies code
                significantly.
             */
            template<typename T>
            std::shared_ptr<T> GetArgument(unsigned pos) const {
                assert(pos < arguments.size());
                return std::dynamic_pointer_cast<T>(arguments[pos]);
            }

            Tensor::Indices GetIndices(unsigned pos) const {
                assert(pos < arguments.size());
                assert(arguments[pos]->IsIndexArgument());
                return GetArgument<IndexArgument>(pos)->GetIndices();
            }

            std::string GetString(unsigned pos) const {
                assert(pos < arguments.size());
                assert(arguments[pos]->IsStringArgument());
                return GetArgument<StringArgument>(pos)->GetValue();
            }

            Tensor::Tensor GetTensors(unsigned pos) const {
                assert(pos < arguments.size());
                assert(arguments[pos]->IsTensorArgument());
                return GetArgument<TensorArgument>(pos)->GetTensor();
            }

            Tensor::Substitution GetSubstitution(unsigned pos) const {
                assert(pos < arguments.size());
                assert(arguments[pos]->IsSubstitutionArgument());
                return GetArgument<SubstitutionArgument>(pos)->GetSubstitution();
            }

            Tensor::Scalar GetNumeric(unsigned pos) const {
                assert(pos < arguments.size());
                assert(arguments[pos]->IsNumericArgument());
                return GetArgument<NumericArgument>(pos)->GetValue();
            }
        public:
            virtual std::string ToLaTeX(const std::vector<std::string>& args) const {
                return "";
            }
        public:
            /**
                \brief Validates the arguments

                This method checks if the arguments are correct. It first
                checks if the sizes match, otherwise throws an WrongNumberOfArgumentsException.
                Then it iterates over all arguments and checks if the types are correct.
                If this is not the case it throws a WrongArgumentTypeException.

                \throws WrongNumberOfArgumentsException
                \throws WrongArgumentTypeException
             */
            void ValidateArguments() const {
                // If sizes do not match return false
                if (ArgumentDictionary::Instance()->IsRepeatedArgument(name)) {
                    if (arguments.size() < ArgumentDictionary::Instance()->Size(name)) {
                        throw WrongNumberOfArgumentsException();
                    }
                } else {
                    if (arguments.size() != ArgumentDictionary::Instance()->Size(name)) {
                        throw WrongNumberOfArgumentsException();
                    }
                }

                for (int i=0; i<arguments.size(); i++) {
                    if (!ArgumentDictionary::Instance()->IsA(name, i, arguments[i]->GetType())) {
                        throw WrongArgumentTypeException(
                                ArgumentDictionary::Instance()->TypeToString(ArgumentDictionary::Instance()->Type(name, i)),
                                ArgumentDictionary::Instance()->TypeToString(arguments[i]->GetType())
                        );
                    }
                }
            }
        public:
            /**
                Handy method to automatically validate the arguments and
                then execute the command

                \throws WrongNumberOfArgumentsException
                \throws WrongArgumentTypeException
             */
            Expression operator()() const {
                ValidateArguments();
                return Execute();
            }
        public:
            static bool Cachable() { return true; }
        public:
            virtual std::string Help() const = 0;
            virtual Expression Execute() const = 0;
        protected:
            std::string name;

            std::vector<ArgumentPointer> arguments;
        };

        typedef std::shared_ptr<Command>    CommandPointer;

        /**
            \class CommandFactory

            The CommandFactory class is responsible for management of
            all known commands. The registration of a command can be
            done manually with the help of the RegisterCommand
            method but it is sincerely recommended to use the
            REGISTER_COMMAND(...) macro for this that employs the
            Registrar class.
         */
        class CommandManagement : public Singleton<CommandManagement> {
        public:
            /**
                Creates a pointer to a command of the given name.
                If the name is not registered in the list so far,
                it throws an UnknownCommandException.

                \throws UnknownCommandException
             */
            std::shared_ptr<Command> CreateCommand(const std::string& name) {
                Command* pointer = nullptr;

                auto it = factory.find(name);
                if (it != factory.end()) pointer = it->second();

                if (pointer != nullptr) {
                    return std::shared_ptr<Command>(pointer);
                } else throw UnknownCommandException();
            }

            /**
                Register a command with the given name into the database.
                The second argument is a lambda that creates a pointer to
                a new command of the given class.
             */
            void RegisterCommand(const std::string& name, const std::function<Command*(void)>& fn) {
                factory[name] = fn;
                names.push_back(name);
            }

            /**
                Returns a list of all the commands
             */
            std::vector<std::string>& GetCommandList() {
                return names;
            }
        private:
            std::map<std::string, std::function<Command*(void)>> factory;
            std::vector<std::string> names;
        };

        /**
            \class CommandRegistrar

            Automatically registers the command in the template T with the
            given name in the CommandManagement. It is sincerely recommended
            to use the REGISTER_COMMAND macro instead.
         */
        template<typename T>
        class CommandRegistrar {
        public:
            CommandRegistrar(const std::string& name) {
                CommandManagement::Instance()->RegisterCommand(name, [](void) -> Command* {
                   return new T();
                });
            }
        };

#define REGISTER_COMMAND(name) static CommandRegistrar<name##Command> registrar_command_##name (#name);
#define CLI_COMMAND(name) class name##Command : public Command { \
public: \
    name##Command() : Command(#name) { }    \
public:

    }
}
