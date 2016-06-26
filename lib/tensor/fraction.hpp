#pragma once

#include <sstream>
#include <string>

#include <tensor/scalar.hpp>

namespace Construction {
    namespace Tensor {

        class Fraction : public AbstractScalar {
        public:
            Fraction() : numerator(0), denominator(1), AbstractScalar(AbstractScalar::FRACTION) { }
            Fraction(int number) :  AbstractScalar(AbstractScalar::FRACTION), numerator(number), denominator(1) { }
            Fraction(int numerator, unsigned int denominator) :  AbstractScalar(AbstractScalar::FRACTION), numerator(numerator), denominator(denominator) { }
        public:
            int gcd(int num1, int num2) {
                int tmp;
                num1 = abs(num1);
                num2 = abs(num2);
                while (num1 > 0) {
                    tmp = num1;
                    num1 = num2 % num1;
                    num2 = tmp;
                }
                return num2;
            }

            void Reduce() {
                int g = gcd(numerator, denominator);
                numerator /= g;
                denominator /= g;
            }
        public:
            bool operator==(const Fraction& other) const {
                return numerator*other.denominator == denominator * other.numerator;
            }

            bool operator==(double other) const {
                return (static_cast<double>(numerator)/denominator) == other;
            }

            bool operator!=(const Fraction& other) const {
                return numerator*other.denominator != denominator * other.numerator;
            }

            bool operator!=(double other) const {
                return (static_cast<double>(numerator)/denominator) != other;
            }

            bool operator<(const Fraction& other) const {
                return numerator*other.denominator < denominator * other.numerator;
            }

            bool operator>(const Fraction& other) const {
                return numerator*other.denominator > denominator * other.numerator;
            }

            bool operator<=(const Fraction& other) const {
                return numerator*other.denominator <= denominator * other.numerator;
            }

            bool operator>=(const Fraction& other) const {
                return numerator*other.denominator >= denominator * other.numerator;
            }

            Fraction& operator+=(const Fraction& other) {
                unsigned int d = denominator * other.denominator;
                int n = numerator * other.denominator + other.numerator * denominator;
                denominator = d;
                numerator = n;
                return *this;
            }

            Fraction& operator-=(const Fraction& other) {
                unsigned int d = denominator * other.denominator;
                int n = numerator * other.denominator - other.numerator * denominator;
                denominator = d;
                numerator = n;
                return *this;
            }

            Fraction& operator*=(const Fraction& other) {
                numerator   *= other.numerator;
                denominator *= other.denominator;
                return *this;
            }

            Fraction& operator/=(const Fraction& other) {
                numerator   *= other.denominator;
                denominator *= other.numerator;
                return *this;
            }

            Fraction operator-() const {
                return Fraction(-numerator, denominator);
            }

            Fraction operator+(const Fraction& other) const {
                unsigned int d = denominator * other.denominator;
                int n = numerator * other.denominator + other.numerator * denominator;
                return Fraction(n,d);
            }

            inline Fraction operator+(int i) const { return *this + Fraction(i); }

            Fraction operator-(const Fraction& other) const {
                unsigned int d = denominator * other.denominator;
                int n = numerator * other.denominator - other.numerator * denominator;
                return Fraction(n,d);
            }

            inline Fraction operator-(int i) const { return *this - Fraction(i); }

            Fraction operator*(const Fraction& other) const {
                return Fraction(numerator * other.numerator, denominator * other.denominator);
            }

            inline Fraction operator*(int i) const { return Fraction(numerator * i, denominator); }

            Fraction operator/(const Fraction& other) const {
                return Fraction(numerator * other.denominator, denominator * other.numerator);
            }

            inline Fraction operator/(int i) const { return Fraction(numerator, denominator * i); }

            operator double() {
                return static_cast<double>(numerator) / denominator;
            }

            virtual double ToDouble() const override {
                return static_cast<double>(numerator) / denominator;
            }

            virtual std::string ToString() const override {
                Fraction c = *this;
                c.Reduce();
                std::stringstream ss;
                ss << c.numerator << "/" << c.denominator;
                return ss.str();
            }

            virtual ScalarPointer Clone() const override {
                return std::move(ScalarPointer(new Fraction(numerator, denominator)));
            }
        public:
            virtual void Serialize(std::ostream& os) const {
                // Call parent
                AbstractScalar::Serialize(os);

                os.write(reinterpret_cast<const char*>(&numerator), sizeof(numerator));
                os.write(reinterpret_cast<const char*>(&denominator), sizeof(denominator));
            }

            static std::unique_ptr<AbstractScalar> Deserialize(std::istream& is) {
                // Call parent
                AbstractScalar::Deserialize(is);

                int numerator;
                unsigned int denominator;
                is.read(reinterpret_cast<char*>(&numerator), sizeof(numerator));
                is.read(reinterpret_cast<char*>(&denominator), sizeof(denominator));

                return std::move(std::unique_ptr<AbstractScalar>(new Fraction(numerator, denominator)));
            }
        private:
            int numerator;
            unsigned int denominator;
        };

    }
}
