#pragma once

#include <common/singleton.hpp>
#include <tensor/tensor.hpp>
#include <tensor/substitution.hpp>

namespace Construction {
    namespace Language {

        enum class ArgumentType {
            INDEX = 101,
            TENSOR = 102,
            NUMERIC = 103,
            STRING = 104,
            SUBSTITUTION = 105,

            UNKNOWN = -1
        };

        class BaseArgument {
        public:
            BaseArgument(ArgumentType type) : type(type) { }
        public:
            ArgumentType GetType() const { return type; }

            bool IsIndexArgument() const { return type == ArgumentType::INDEX; }
            bool IsTensorArgument() const { return type == ArgumentType::TENSOR; }
            bool IsNumericArgument() const { return type == ArgumentType::NUMERIC; }
            bool IsStringArgument() const { return type == ArgumentType::STRING; }
            bool IsSubstitutionArgument() const { return type == ArgumentType::SUBSTITUTION; }
        public:
            virtual void Parse(const std::string& code) = 0;
        private:
            ArgumentType type;
        };

        typedef std::shared_ptr<BaseArgument>   ArgumentPointer;

        class ArgumentDictionary : public Singleton<ArgumentDictionary> {
        public:
            bool IsA(const std::string& name, int pos, ArgumentType type) const {
                auto it = factory.find(name);
                if (it == factory.end()) return false;

                // If pos is larger than the number of registered arguments, set to last
                if (it->second.second && pos >= it->second.first.size()) {
                    pos = it->second.first.size()-1;
                }

                auto iter = it->second.first.find(pos);
                if (iter == it->second.first.end()) return false;

                return iter->second == type;
            }

            bool IsRepeatedArgument(const std::string& name) const {
                auto it = factory.find(name);
                if (it == factory.end()) return false;
                return it->second.second;
            }

            ArgumentType Type(const std::string& name, int pos) const {
                auto it = factory.find(name);
                if (it == factory.end()) return ArgumentType::UNKNOWN;

                // If pos is larger than the number of registered arguments, set to last
                if (it->second.second && pos >= it->second.first.size()) {
                    pos = it->second.first.size()-1;
                }

                auto iter = it->second.first.find(pos);
                if (iter == it->second.first.end()) return ArgumentType::UNKNOWN;

                return iter->second;
            }

            static std::string TypeToString(ArgumentType type) {
                switch (type) {
                    case ArgumentType::INDEX: return "Indices";
                    case ArgumentType::TENSOR: return "Tensor";
                    case ArgumentType::NUMERIC: return "Numeric";
                    case ArgumentType::STRING: return "String";
                    case ArgumentType::SUBSTITUTION: return "Substitution";

                    default: return "Unknown";
                }
            }

            size_t Size(const std::string& name) const {
                auto it = factory.find(name);
                if (it == factory.end()) return 0;

                return it->second.first.size();
            }

            void RegisterArgument(const std::string& name, int pos, ArgumentType type, bool repeatLast=false) {
                factory[name].first[pos] = type;
                factory[name].second = repeatLast;
            }
        private:
            std::map<std::string, std::pair<std::map<int, ArgumentType>, bool> > factory;
        };

        template<int number, ArgumentType type>
        class Argument {
        public:
            Argument(const std::string& name, bool repeatLast=false) {
                ArgumentDictionary::Instance()->RegisterArgument(name, number, type, repeatLast);
            }
        };

        typedef std::vector<std::shared_ptr<BaseArgument>> Arguments;

#define REGISTER_ARGUMENT(name, pos, type) static Argument<pos, type> name##_arg##pos (#name)
#define REGISTER_REPEATED_ARGUMENT(name, pos, type) static Argument<pos, type> name##_arg##pos (#name, true)

        /**
            \class IndexArgument

            Allows to enter indices. An index argument always starts
            and ends with " ". Between the indices there has to be
            one or more whitespace(s). For example
                "a b c"
            is a proper index argument. But
                "abc"
            will throw an error.
         */
        class IndexArgument : public BaseArgument {
        public:
            IndexArgument(const std::string& code) : BaseArgument(ArgumentType::INDEX) {
                Parse(code);
            }

            IndexArgument(const Tensor::Indices& indices) : BaseArgument(ArgumentType::INDEX), indices(indices) { }
        public:
            inline Tensor::Indices GetIndices() const { return indices; }
        public:
            virtual void Parse(const std::string& code) {
                indices.Clear();

                std::string current;

                for (int pos=0; pos<code.length(); pos++) {
                    char c = code[pos];

                    if (c == ' ') {
                        if (current.length() == 0) break;

                        Tensor::Index index(current, current, {1,3});
                        if (indices.ContainsIndex(index)) {
                            // throw error
                            return;
                        }

                        indices.Insert(index);
                        current = "";
                    } else {
                        current.append(std::string(1,c));
                    }
                }

                if (current.length() != 0) {
                    Tensor::Index index(current, current, {1, 3});
                    if (indices.ContainsIndex(index)) {
                        // throw error
                        return;
                    }

                    indices.Insert(index);
                }
            }
        private:
            Tensor::Indices indices;
        };

        class TensorArgument : public BaseArgument {
        public:
            TensorArgument() : BaseArgument(ArgumentType::TENSOR) { }
        public:
            inline void SetTensor(const Tensor::Tensor& container) {
                result = container;
            }

            inline Tensor::Tensor GetTensor() const {
                return result;
            }
        public:
            virtual void Parse(const std::string& code) { }
        private:
            Tensor::Tensor result;
        };

        class NumericArgument : public BaseArgument {
        public:
            NumericArgument(const std::string& text) : BaseArgument(ArgumentType::NUMERIC) {
                if (text.find(".") == std::string::npos) {
                    value = Tensor::Scalar::Fraction(std::atoi(text.c_str()), 1);
                } else {
                    value = Tensor::Scalar::Fraction(std::atof(text.c_str()));
                }
            }
            NumericArgument(const Tensor::Scalar& value) : BaseArgument(ArgumentType::NUMERIC), value(value) { }
        public:
            inline void SetValue(const Tensor::Scalar& v) {
                value = v;
            }

            inline Tensor::Scalar GetValue() const {
                return value;
            }
        public:
            virtual void Parse(const std::string& code) { }
        private:
            Tensor::Scalar value;
        };

        class StringArgument : public BaseArgument {
        public:
            StringArgument(const std::string& text) : BaseArgument(ArgumentType::STRING), value(text) { }
        public:
            inline void SetValue(const std::string& v) { value = v; }
            inline std::string GetValue() const { return value; }
        public:
            virtual void Parse(const std::string& code) { }
        public:
            std::string value;
        };

        class SubstitutionArgument : public BaseArgument {
        public:
            SubstitutionArgument() : BaseArgument(ArgumentType::SUBSTITUTION) { }
        public:
            inline void SetSubstitution(const Tensor::Substitution& substitution) {
                this->substitution = substitution;
            }

            inline Tensor::Substitution GetSubstitution() const { return substitution; }
        public:
            virtual void Parse(const std::string& code) { }
        private:
            Tensor::Substitution substitution;
        };

    }
}