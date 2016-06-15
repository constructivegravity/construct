#pragma once

#include <string>
#include <sstream>

namespace Construction {
    namespace Tensor {

		class AddedScalar;
		class MultipliedScalar;

        /**
            \class Scalar


         */
        class Scalar {
        public:
            enum Type {
                // Fundamentals
                VARIABLE = 1,
                FRACTION = 2,
                FLOATING_POINT = 3,

                ADDED = 101,
                MULTIPLIED = 102
            };
        public:
            Scalar() : type(FLOATING_POINT) { }
            Scalar(Type type) : type(type) { }
        public:
            Type GetType() const { return type; }

            bool IsVariable() const { return type == VARIABLE; }
            bool IsFraction() const { return type == FRACTION; }
            bool IsFloatingPoint() const { return type == FLOATING_POINT; }

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
            virtual std::shared_ptr<Scalar> Clone() const {
				return std::make_shared<Scalar>(*this);
			}
        public:
            friend std::shared_ptr<Scalar> Add(const Scalar& one, const Scalar& other);
            friend std::shared_ptr<Scalar> Subtract(const Scalar& one, const Scalar& other);
            friend std::shared_ptr<Scalar> Multiply(const Scalar& one, const Scalar& other);
            friend std::shared_ptr<Scalar> Negate(const Scalar& one);

            /*Scalar& operator+(const Scalar& other) const;
            Scalar& operator*(const Scalar& other) const;

            Scalar& operator*(double c) const;
            friend inline Scalar& operator*(double c, const Scalar& other);

            inline Scalar& operator-(const Scalar& other) const;

            Scalar& operator-() const;*/
        public:
            /**
                Replace a variable by another scalar
             */
            virtual void Replace() {

            }
        protected:
            Type type;
        };

        typedef std::shared_ptr<Scalar> ScalarPointer;

        class FloatingPointScalar : public Scalar {
        public:
            FloatingPointScalar() : Scalar(FLOATING_POINT), c(0) { }
            FloatingPointScalar(double c) : Scalar(FLOATING_POINT), c(c) { }
        public:
            virtual ScalarPointer Clone() const override {
                return std::make_shared<FloatingPointScalar>(c);
            }
        public:
            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << c;
                return ss.str();
            }

            operator double() const { return c; }

            virtual double ToDouble() const override { return c; }
        private:
            double c;
        };

        class AddedScalar : public Scalar {
        public:
            AddedScalar(ScalarPointer A, ScalarPointer B) : A(A), B(B) {
                type = ADDED;
            }
        public:
            virtual ScalarPointer Clone() const override {
				return std::make_shared<AddedScalar>(
					std::move(A->Clone()),
					std::move(B->Clone())
				);
			}
        public:
            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << A->ToString() << " + " << B->ToString();
                return ss.str();
            }
        public:
            inline ScalarPointer GetFirst() const { return A; }
            inline ScalarPointer GetSecond() const { return B; }
        private:
            ScalarPointer A;
            ScalarPointer B;
        };

        class MultipliedScalar : public Scalar {
        public:
            MultipliedScalar(ScalarPointer A, ScalarPointer B) : A(A), B(B) {
                type = MULTIPLIED;
            }
        public:
            virtual std::string ToString() const override {
                std::stringstream ss;
                ss << A->ToString() << " * " << B->ToString();
                return ss.str();
            }
        private:
            ScalarPointer A;
            ScalarPointer B;
        };

    }
}
