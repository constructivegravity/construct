#pragma once

#include <sstream>
#include <string>
#include <iostream>

#include <tensor/scalar.hpp>

namespace Construction {
    namespace Tensor {

        class Fraction : public AbstractScalar {
        public:
            Fraction() : numerator(0), denominator(1), AbstractScalar(AbstractScalar::FRACTION) { }
            Fraction(int number) :  AbstractScalar(AbstractScalar::FRACTION), numerator(number), denominator(1) { }
            Fraction(long numerator, long denominator) :  AbstractScalar(AbstractScalar::FRACTION), numerator(numerator), denominator(denominator) {
                if (this->denominator < 0) {
                    this->numerator = -this->numerator;
                    this->denominator = -this->denominator;
                }
            }

            virtual ~Fraction() = default;
        public:
            long gcd(long num1, long num2) {
                long tmp;
                num1 = std::abs(num1);
                num2 = std::abs(num2);
                while (num1 > 0) {
                    tmp = num1;
                    num1 = num2 % num1;
                    num2 = tmp;
                }
                return num2;
            }

            void Reduce() {
                if (numerator == 0) return;

                long g = gcd(numerator, denominator);
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
                long d = denominator * other.denominator;
                long n = numerator * other.denominator + other.numerator * denominator;
                denominator = d;
                numerator = n;
                Reduce();
                return *this;
            }

            Fraction& operator-=(const Fraction& other) {
                long d = denominator * other.denominator;
                long n = numerator * other.denominator - other.numerator * denominator;
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
                long factor = ((numerator < 0 && other.numerator > 0) || (numerator > 0 && other.numerator < 0)) ? -1 : 1;
                long newNumerator = factor * std::abs(numerator) * other.denominator;
                long newDenominator = denominator * std::abs(other.numerator);

                numerator   = newNumerator;
                denominator = newDenominator;
                return *this;
            }

            Fraction operator-() const {
                return Fraction(-numerator, denominator);
            }

            Fraction operator+(const Fraction& other) const {
                long d = denominator * other.denominator;
                long n = numerator * other.denominator + other.numerator * denominator;
                Fraction result (n,d);
                result.Reduce();
                return result;
            }

            inline Fraction operator+(int i) const { return *this + Fraction(i); }

            Fraction operator-(const Fraction& other) const {
                long d = denominator * other.denominator;
                long n = numerator * other.denominator - other.numerator * denominator;
                Fraction result (n,d);
                result.Reduce();
                return result;
            }

            inline Fraction operator-(int i) const { return *this - Fraction(i); }

            Fraction operator*(const Fraction& other) const {
                Fraction result (numerator * other.numerator, denominator * other.denominator);
                result.Reduce();
                return result;
            }

            inline Fraction operator*(int i) const { return Fraction(numerator * i, denominator); }

            Fraction operator/(const Fraction& other) const {
                long factor = ((numerator < 0 && other.numerator > 0) || (numerator > 0 && other.numerator < 0)) ? -1 : 1;
                long newNumerator = factor * std::abs(numerator) * other.denominator;
                long newDenominator = denominator * std::abs(other.numerator);

                Fraction result (newNumerator, newDenominator);
                result.Reduce();
                return result;
            }

            inline Fraction operator/(int i) const {
                long factor = (i < 0) ? -1 : 1;
                long newNumerator = factor * numerator;
                long newDenominator = denominator * i;

                Fraction result(newNumerator, newDenominator);
                result.Reduce();
                return result;
            }

            operator double() {
                return static_cast<double>(numerator) / denominator;
            }

            virtual double ToDouble() const override {
                return static_cast<double>(numerator) / denominator;
            }

            virtual std::string ToString() const override {
                // Do not write 0 to complicated
                if (numerator == 0) return "0";

                // Reduce
                Fraction c = *this;
                c.Reduce();

                std::stringstream ss;
                if (c.denominator == 1) ss << c.numerator;
                else ss << c.numerator << "/" << c.denominator;
                return ss.str();
            }

            friend std::ostream& operator<<(std::ostream& os, const Fraction& fraction) {
                os << fraction.ToString();
                return os;
            }

            virtual ScalarPointer Clone() const override {
                return std::move(ScalarPointer(new Fraction(numerator, denominator)));
            }
        public:
            virtual void Serialize(std::ostream& os) const override {
                // Call parent
                AbstractScalar::Serialize(os);

                WriteBinary<long>(os, numerator);
                WriteBinary<long>(os, denominator);
            }

            static std::unique_ptr<AbstractScalar> Deserialize(std::istream& is) {
                // Call parent
                AbstractScalar::Deserialize(is);

                long numerator = ReadBinary<long>(is);
                long denominator = ReadBinary<long>(is);

                return std::move(std::unique_ptr<AbstractScalar>(new Fraction(numerator, denominator)));
            }
        public:
            static Fraction FromDouble(double f) {
                if (f < 0) return -FromDouble(-f);

                std::vector<int> values;

                int integer = static_cast<int>(f);
                double rest = f - integer;

                values.push_back(integer);
                int counter = 0;

                while (rest != 0 && rest > 1e-6) {
                    double x = 1.0/rest;

                    integer = static_cast<int>(x);
                    double diff = 1-(x - static_cast<int>(x));
                    if (diff < 1e-6) ++integer;

                    rest = x - integer;

                    values.push_back(integer);
                }

                // Put together
                Fraction result (values[values.size()-1],1);

                for (int i=values.size()-2; i>=0; --i) {
                    // Invert the value
                    result = Fraction(1,1) / result;

                    // Add the current value
                    result += Fraction(values[i],1);
                }

                return result;
            }
        private:
            long numerator;
            long denominator;
        };

    }
}
