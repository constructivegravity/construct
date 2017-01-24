#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>

#include <common/printable.hpp>
#include <common/serializable.hpp>

#include <tensor/expression.hpp>

namespace Construction {
    namespace Tensor {

		class AddedScalar;
		class MultipliedScalar;

        using Common::Printable;
        using Common::Serializable;

        /**
            \class Scalar

            \brief Class for general scalars
         */
        class AbstractScalar : public Serializable<AbstractScalar> {
        public:
            enum Type {
                // Fundamentals
                VARIABLE = 1,
                FRACTION = 2,
                FLOATING_POINT = 3,

                // Arithmetic types
                ADDED = 101,
                MULTIPLIED = 102
            };
        public:
            /**
                Constructor of a scalar
             */
            AbstractScalar() : type(FLOATING_POINT) { }

            /**
                Constructor of a scalar

                \param type     The type of the scalar
             */
            AbstractScalar(Type type) : type(type) { }

            virtual ~AbstractScalar() { }
        public:
            /**
             *  Return the type of the scalar
             */
            Type GetType() const { return type; }

            /**
                Return if the scalar is a variable
             */
            bool IsVariable() const { return type == VARIABLE; }
            bool IsFraction() const { return type == FRACTION; }
            bool IsFloatingPoint() const { return type == FLOATING_POINT; }
            bool IsNumeric() const { return type == FLOATING_POINT || type == FRACTION; }

            bool IsAdded() const { return type == ADDED; }
            bool IsMultiplied() const { return type == MULTIPLIED; }

            std::string TypeToString() const {
                switch (type) {
                    case VARIABLE: return "Variable";
                    case FRACTION: return "Fraction";
                    case FLOATING_POINT: return "Floating Point";

                    case ADDED: return "Added";
                    case MULTIPLIED: return "Multiplied";
                    default: return "Unknown";
                }
            }
        public:
            virtual std::string ToString() const {
                return "";
            }

            virtual double ToDouble() const {
                return 0;
            }
        public:
            virtual std::unique_ptr<AbstractScalar> Clone() const = 0;
        public:
            /** Arithmetics **/

            static std::unique_ptr<AbstractScalar> Add(const AbstractScalar& one, const AbstractScalar& other);
            static std::unique_ptr<AbstractScalar> Subtract(const AbstractScalar& one, const AbstractScalar& other);
            static std::unique_ptr<AbstractScalar> Multiply(const AbstractScalar& one, const AbstractScalar& other);
            static std::unique_ptr<AbstractScalar> Negate(const AbstractScalar& one);
        public:
            /**
                Extract pointers to all the free variables in
                this scalar expression
             */
            std::vector<std::unique_ptr<AbstractScalar>> GetVariables() const;
            bool HasVariables() const;
        public:
            /**
                Take the expression and replace the given variable by a numeric
             */
            //virtual void Replace(const Variable& variable, const Scalar& scalar);
        public:
            void Serialize(std::ostream& os) const override {
                WriteBinary(os, static_cast<unsigned>(type));
            }

            static std::unique_ptr<AbstractScalar> Deserialize(std::istream& is) { return nullptr; }
        protected:
            Type type;
        };

        typedef std::unique_ptr<AbstractScalar> ScalarPointer;
        typedef const std::unique_ptr<AbstractScalar> ConstScalarPointer;

        class FloatingPointScalar : public AbstractScalar {
        public:
            FloatingPointScalar() : AbstractScalar(FLOATING_POINT), c(0) { }
            FloatingPointScalar(double c) : AbstractScalar(FLOATING_POINT), c(c) { }

            virtual ~FloatingPointScalar() = default;
        public:
            virtual ScalarPointer Clone() const override {
                return std::move(ScalarPointer(new FloatingPointScalar(c)));
            }
        public:
            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << c;
                return ss.str();
            }

            operator double() const { return c; }

            virtual double ToDouble() const override { return c; }
        public:
            virtual void Serialize(std::ostream& os) const override {
                // Call parent
                AbstractScalar::Serialize(os);

                WriteBinary(os, c);
            }

            static std::unique_ptr<AbstractScalar> Deserialize(std::istream& is) {
                double c = ReadBinary<double>(is);

                return ScalarPointer(new FloatingPointScalar(c));
            }
        public:
            friend class AbstractScalar;
        private:
            double c;
        };

        class AddedScalar : public AbstractScalar {
        public:
            AddedScalar(ScalarPointer A, ScalarPointer B) : AbstractScalar(ADDED), A(std::move(A)), B(std::move(B)) { }

            virtual ~AddedScalar() = default;
        public:
            virtual ScalarPointer Clone() const override {
                return std::move(ScalarPointer(new AddedScalar(
                    std::move(A->Clone()),
                    std::move(B->Clone())
                )));
			}
        public:
            virtual std::string ToString() const override {
                std::stringstream ss;
                auto s = B->ToString();

                if (B->IsMultiplied()) {
                    if (s[0] == '-') {
                        ss << A->ToString() << " - " << s.substr(1);
                        return ss.str();
                    }
                }

                ss << A->ToString() << " + " << s;
                return ss.str();
            }
        public:
            virtual void Serialize(std::ostream& os) const override {
                // Call parent
                AbstractScalar::Serialize(os);

                A->Serialize(os);
                B->Serialize(os);
            }

            static std::unique_ptr<AbstractScalar> Deserialize(std::istream& is) {
                // do nothing
                return nullptr;
            }

            inline const ScalarPointer& GetFirst() const { return A; }
            inline const ScalarPointer& GetSecond() const { return B; }

            /*inline ConstScalarPointer GetFirst() const { return A; }
            inline ConstScalarPointer GetSecond() const { return B; }*/
        public:
            friend class AbstractScalar;
        private:
            ScalarPointer A;
            ScalarPointer B;
        };

        class MultipliedScalar : public AbstractScalar {
        public:
            MultipliedScalar(ScalarPointer A, ScalarPointer B) : AbstractScalar(MULTIPLIED), A(std::move(A)), B(std::move(B)) { }
            virtual ~MultipliedScalar() = default;
        public:
            virtual std::string ToString() const override {
                std::stringstream ss;

                // If one is minus one, just return the negated expression
                if (A->IsNumeric() && A->ToDouble() == -1) {
                    if (B->IsAdded()) {
                        ss << "-(" << B->ToString() << ")";
                    } else {
                        ss << "-" << B->ToString();
                    }
                    return ss.str();
                }

                if (B->IsNumeric() && B->ToDouble() == -1) {
                    if (A->IsAdded()) {
                        ss << "-(" << A->ToString() << ")";
                    } else {
                        ss << "-" << A->ToString();
                    }
                    return ss.str();
                }

                // Factorize if necessary
                if (A->IsAdded()) {
                    ss << "(" << A->ToString() << ")";
                } else ss << A->ToString();

                ss << " * ";

                if (B->IsAdded()) {
                    ss << "(" << B->ToString() << ")";
                } else ss << B->ToString();

                return ss.str();
            }
        public:
            inline const ScalarPointer& GetFirst() const { return A; }
            inline const ScalarPointer& GetSecond() const { return B; }
        public:
            virtual ScalarPointer Clone() const override {
                return std::move(ScalarPointer(new MultipliedScalar(
                    std::move(A->Clone()),
                    std::move(B->Clone())
                )));
            }

            virtual void Serialize(std::ostream& os) const override {
                // Call parent
                AbstractScalar::Serialize(os);

                A->Serialize(os);
                B->Serialize(os);
            }

            static std::unique_ptr<AbstractScalar> Deserialize(std::istream& is) {
                // do nothing
                return nullptr;
            }
        public:
            friend class AbstractScalar;
        private:
            ScalarPointer A;
            ScalarPointer B;
        };

        /**
            \class Scalar

            Syntactic sugar class for scalars. This allows to really just add, multiply etc.
            them without having to use the pointers and worrying about what happens under
            the surface. This will greatly simplify the work with the scalars!
         */
        class Scalar : public AbstractExpression {
        public:
            Scalar();
            Scalar(double v);

            Scalar(int v);
            Scalar(int numerator, unsigned denominator);

            Scalar(const std::string& name);
            Scalar(const std::string& name, const std::string& printed_text);
            Scalar(const std::string& name, unsigned id);

            Scalar(const Scalar& other) : pointer(std::move(other.pointer->Clone())) { }
            Scalar(Scalar&& other) : pointer(std::move(other.pointer)) { }

            Scalar(std::unique_ptr<AbstractScalar> pointer) : pointer(std::move(pointer)) { }

            virtual ~Scalar() = default;
        public:
            // Syntactic sugar
            inline static Scalar Integer(int v) { return Scalar(v); }
            inline static Scalar Fraction(int numerator, unsigned denominator) { return Scalar(numerator, denominator); }
            static Scalar Fraction(double f);
            inline static Scalar FloatingPoint(double v) { return Scalar(v); }

            inline static Scalar Variable(const std::string& name) { return Scalar(name); }
            inline static Scalar Variable(const std::string& name, const std::string& printed_text) { return Scalar(name, printed_text); }
            inline static Scalar Variable(const std::string& name, unsigned id) { return Scalar(name, id); }
        public:
            Scalar& operator=(const Scalar& other) {
                pointer = other.pointer->Clone();
                return *this;
            }

            Scalar& operator=(Scalar&& other) {
                pointer = std::move(other.pointer);
                return *this;
            }

            Scalar& operator=(double d);
        public:
            virtual ExpressionPointer Clone() const override { return std::move(ExpressionPointer(new Scalar(*this))); }

            virtual bool IsScalarExpression() const override { return true; }
        public:
            inline AbstractScalar::Type GetType() const { return pointer->GetType(); }
            inline std::string TypeToString() const { return pointer->TypeToString(); }

            inline bool IsVariable() const { return pointer->IsVariable(); }
            inline bool IsFraction() const { return pointer->IsFraction(); }
            inline bool IsFloatingPoint() const { return pointer->IsFloatingPoint(); }
            inline bool IsNumeric() const { return pointer->IsNumeric(); }

            inline bool IsAdded() const { return pointer->IsAdded(); }
            inline bool IsMultiplied() const { return pointer->IsMultiplied(); }
        public:
            inline bool HasVariables() const { return pointer->HasVariables(); }
            inline std::vector<Scalar> GetVariables() const {
                std::vector<Scalar> result;
                auto variables = pointer->GetVariables();
                for (auto& variable : variables) {
                    result.push_back(Scalar(std::move(variable)));
                }
                return result;
            }
        public:
            Scalar& operator+=(const Scalar& other) {
                ScalarPointer old = std::move(pointer);
                pointer = std::move(AbstractScalar::Add(*old, *other.pointer));
                return *this;
            }

            Scalar operator+(const Scalar& other) const {
                return Scalar(std::move(AbstractScalar::Add(*pointer, *other.pointer)));
            }

            Scalar& operator*=(const Scalar& other) {
                ScalarPointer old = std::move(pointer);
                pointer = std::move(AbstractScalar::Multiply(*old, *other.pointer));
                return *this;
            }

            Scalar operator*(const Scalar& other) const {
                return Scalar(std::move(AbstractScalar::Multiply(*pointer, *other.pointer)));
            }

            Scalar operator-() const {
                return Scalar(std::move(AbstractScalar::Negate(*pointer)));
            }

            Scalar& operator-=(const Scalar& other) {
                ScalarPointer old = std::move(pointer);
                pointer = std::move(AbstractScalar::Add(*old, *AbstractScalar::Negate(*other.pointer)));
                return *this;
            }

            inline Scalar operator-(const Scalar& other) const {
                return (*this) + (-other);
            }
        public:
            template<class T>
            const T* As() const {
                return static_cast<const T*>(pointer.get());
            }
        public:
            bool operator==(const Scalar& other) const;

            inline bool operator!=(const Scalar& other) const {
                return !((*this) == other);
            }

            bool operator<(const Scalar& other) const {
                return ToDouble() < other.ToDouble();
            }

            bool operator<=(const Scalar& other) const {
                return ToDouble() <= other.ToDouble();
            }

            bool operator>(const Scalar& other) const {
                return ToDouble() > other.ToDouble();
            }

            bool operator>=(const Scalar& other) const {
                return ToDouble() >= other.ToDouble();
            }
        public:
            virtual std::string ToString() const override {
                return pointer->ToString();
            }

            inline double ToDouble() const {
                return pointer->ToDouble();
            }
        public:
            std::vector<Scalar> GetSummands() const {
                // Helper method
                std::function<std::vector<Scalar>(const AbstractScalar*)> helper = [&](const AbstractScalar* scalar) {
                    std::vector<Scalar> result;

                    if (scalar->IsAdded()) {
                        // Recursively look at the leafs from the sum node
                        auto left = helper(static_cast<const AddedScalar*>(scalar)->GetFirst().get());
                        auto right = helper(static_cast<const AddedScalar*>(scalar)->GetSecond().get());

                        // Add the found tensors to the result
                        for (auto& item : left) result.push_back(item);
                        for (auto& item : right) result.push_back(item);
                    } else {
                        result.push_back(Scalar(ScalarPointer(std::move(scalar->Clone()))));
                    }

                    return result;
                };

                // Execute
                return helper(pointer.get());
            }

            /**
                \brief Expand scalar expressions
             */
            Scalar Expand() const {
                if (pointer->IsAdded()) {
                    auto summands = GetSummands();
                    Scalar result;

                    for (auto& scalar : summands) {
                        auto expanded = scalar.Expand().GetSummands();

                        for (auto& s : expanded) {
                            result += s;
                        }
                    }

                    return result;
                }

                if (pointer->IsMultiplied()) {
                    auto expandedLeft = Scalar(static_cast<MultipliedScalar*>(pointer.get())->GetFirst()->Clone()).Expand().GetSummands();
                    auto expandedRight = Scalar(static_cast<MultipliedScalar*>(pointer.get())->GetSecond()->Clone()).Expand().GetSummands();

                    Scalar result;

                    for (auto& s : expandedLeft) {
                        for (auto& t : expandedRight) {
                            result += s*t;
                        }
                    }

                    return result;
                }

                return *this;
            }

            /**
                \brief Substitute variables in a scalar expression

                Substitutes a variable with an arbitrary scalar expression. Since
                we build up the result again after identifying the important terms,
                all the black magic for simplification works, i.e.
                terms multiplied by zero will not appear anymore, other terms can
                drop out etc.

                \param {Scalar} variable        The variable to replace
                \param {Scalar} other           The expression to plug in

                \returns Scalar                 The substituted expression
             */
            Scalar Substitute(const Scalar& variable, const Scalar& other) const {
                // If the given scalar is not a variable, return the original scalar
                if (!variable.IsVariable()) return *this;

                // Split into sums
                auto summands = GetSummands();

                Scalar result = 0;

                // Iterate over all summands
                for (auto& s : summands) {
                    // If it is just a number or the searched variable, just add the result
                    if (s.IsVariable()) result += (s == variable) ? other : s;
                    if (s.IsNumeric()) result += s;

                    // If it is a multiplication, substitute in each factor recursively
                    if (s.IsMultiplied()) {
                        result += Scalar(static_cast<MultipliedScalar*>(s.pointer.get())->GetFirst()->Clone()).Substitute(variable, other) *
                                  Scalar(static_cast<MultipliedScalar*>(s.pointer.get())->GetSecond()->Clone()).Substitute(variable, other);
                    }
                }

                return result;
            }

            Scalar FactorizeOveralScale() const {
                Scalar overal = Scalar::Fraction(1,1);

                auto summands = GetSummands();
                bool first=true;
                Scalar result = Scalar::Fraction(0,1);

                for (auto& summand : summands) {
                    auto sep = summand.SeparateVariablesFromRest();

                    for (auto& pair : sep.first) {
                        if (first) {
                            first = false;
                            overal = pair.second;
                        }

                        if (pair.second != overal) return *this;
                        result += pair.first;
                    }

                    // If the rest does not fit, return the original
                    if (sep.second != Scalar::Fraction(0,1)) {
                        if (first) {
                            first = false;
                            overal = sep.second;
                        }

                        if (sep.second != overal) return *this;

                        // Add the one from the rest
                        result += Fraction(1,1);
                    }
                }

                return overal * result;
            }

            std::pair<std::vector<std::pair<Scalar, Scalar>>, Scalar> SeparateVariablesFromRest() const {
                Scalar rest = 0;

                // Get the summands
                auto summands = Expand().GetSummands();

                std::vector<Scalar> keys;
                std::vector<Scalar> values;

                for (auto& s : summands) {
                    // If s is a variable
                    if (s.IsNumeric()) {
                        rest += s;
                        continue;
                    }

                    if (s.IsVariable()) {
                        auto it = std::find(keys.begin(), keys.end(), s);
                        if (it == keys.end()) {
                            keys.push_back(s);
                            values.push_back(Scalar(1));
                        } else {
                            values[it - keys.begin()] += Scalar(1);
                        }

                        continue;
                    }

                    if (s.IsMultiplied()) {
                        auto first = Scalar(static_cast<MultipliedScalar*>(s.pointer.get())->GetFirst()->Clone());
                        auto second = Scalar(static_cast<MultipliedScalar*>(s.pointer.get())->GetSecond()->Clone());

                        if (first.IsVariable()) {
                            auto it = std::find(keys.begin(), keys.end(), first);
                            if (it == keys.end()) {
                                keys.push_back(first);
                                values.push_back(second);
                            } else {
                                values[it - keys.begin()] += second;
                            }

                            continue;
                        }

                        if (second.IsVariable()) {
                            auto it = std::find(keys.begin(), keys.end(), second);
                            if (it == keys.end()) {
                                keys.push_back(second);
                                values.push_back(first);
                            } else {
                                values[it - keys.begin()] += first;
                            }

                            continue;
                        }
                    }
                }

                std::vector<std::pair<Scalar, Scalar>> result;
                for (int i=0; i<keys.size(); ++i) {
                    result.push_back({ keys[i], values[i] });
                }

                return { result, rest };
            }
        public:
            bool IsProportionalTo(const Scalar& other, Scalar* factor = nullptr);
        public:
            void Serialize(std::ostream& os) const override;
            static std::unique_ptr<AbstractExpression> Deserialize(std::istream& is);
        private:
            ScalarPointer pointer;
        };

    }
}

namespace std {

    /**
        Hash function for scalars to use std::unordered_map
     */
    template<>
    struct hash<Construction::Tensor::Scalar> {
        std::size_t operator()(const Construction::Tensor::Scalar& scalar) const {
            return std::hash<std::string>()(scalar.ToString());
        }
    };

}
