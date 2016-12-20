#pragma once

#include <sstream>
#include <string>
#include <iostream>
#include <cassert>
#include <stdexcept>

#include <common/bignumber.hpp>
#include <tensor/scalar.hpp>

namespace Construction {
    namespace Tensor {

        template<typename T>
        class FractionBase : public AbstractScalar {
        public:
            FractionBase() : numerator(T(0)), denominator(T(1)), AbstractScalar(AbstractScalar::FRACTION) { }
            FractionBase(const T& number) :  AbstractScalar(AbstractScalar::FRACTION), numerator(number), denominator(T(1)) { }
            FractionBase(const T& numerator, const T& denominator) :  AbstractScalar(AbstractScalar::FRACTION), numerator(numerator), denominator(denominator) {
                if (this->denominator < T(0)) {
                    this->numerator = -this->numerator;
                    this->denominator = -this->denominator;
                }
            }

            virtual ~FractionBase() = default;
        public:
            T gcd(T num1, T num2) {
                T tmp;

                num1 = (num1 > T(0)) ? num1 : -num1;
                num2 = (num2 > T(0)) ? num2 : -num2;

                while (num1 > T(0)) {
                    tmp = num1;
                    num1 = num2 % num1;
                    num2 = tmp;
                }

                return num2;
            }

            void Reduce() {
                // Beautify a zero
                if (numerator == T(0)) {
                    denominator = 1;
                    return;
                }

                T g = gcd(numerator, denominator);
                numerator /= g;
                denominator /= g;

                if (denominator == T(0)) {
                    std::cout << "What if I told you, dividing by zero IS the problem?" << std::endl;
                }

                assert(denominator != T(0) && "Bro, don't divide by zero");
            }
        public:
            bool operator==(const FractionBase& other) const {
                return numerator*other.denominator == denominator * other.numerator;
            }

            bool operator==(double other) const {
                return (static_cast<double>(numerator)/static_cast<double>(denominator)) == other;
            }

            bool operator!=(const FractionBase& other) const {
                return numerator*other.denominator != denominator * other.numerator;
            }

            bool operator!=(double other) const {
                return (static_cast<double>(numerator)/static_cast<double>(denominator)) != other;
            }

            bool operator<(const FractionBase& other) const {
                return numerator*other.denominator < denominator * other.numerator;
            }

            bool operator>(const FractionBase& other) const {
                return numerator*other.denominator > denominator * other.numerator;
            }

            bool operator<=(const FractionBase& other) const {
                return numerator*other.denominator <= denominator * other.numerator;
            }

            bool operator>=(const FractionBase& other) const {
                return numerator*other.denominator >= denominator * other.numerator;
            }

            FractionBase& operator+=(const FractionBase& other) {
                T d = denominator * other.denominator;
                T n = numerator * other.denominator + other.numerator * denominator;
                denominator = d;
                numerator = n;
                Reduce();
                return *this;
            }

            FractionBase& operator-=(const FractionBase& other) {
                T d = denominator * other.denominator;
                T n = numerator * other.denominator - other.numerator * denominator;
                denominator = d;
                numerator = n;
                Reduce();
                return *this;
            }

            FractionBase& operator*=(const FractionBase& other) {
                numerator   *= other.numerator;
                denominator *= other.denominator;
                Reduce();
                return *this;
            }

            FractionBase& operator/=(const FractionBase& other) {
                T factor = ((numerator < 0 && other.numerator > 0) || (numerator > 0 && other.numerator < 0)) ? -1 : 1;
                T newNumerator = factor * std::abs(numerator) * other.denominator;
                T newDenominator = denominator * std::abs(other.numerator);

                numerator   = newNumerator;
                denominator = newDenominator;
                Reduce();
                return *this;
            }

            FractionBase operator-() const {
                auto result = FractionBase(-numerator, denominator);
                result.Reduce();
                return result;
            }

            FractionBase operator+(const FractionBase& other) const {
                T d = denominator * other.denominator;
                T n = numerator * other.denominator + other.numerator * denominator;
                FractionBase result (n,d);
                result.Reduce();
                return result;
            }

            inline FractionBase operator+(int i) const {
                return *this + FractionBase(i,T(1));
            }

            FractionBase operator-(const FractionBase& other) const {
                T d = denominator * other.denominator;
                T n = numerator * other.denominator - other.numerator * denominator;
                FractionBase result (n,d);
                result.Reduce();
                return result;
            }

            inline FractionBase operator-(int i) const { return *this - FractionBase(i,T(1)); }

            FractionBase operator*(const FractionBase& other) const {
                FractionBase result (numerator * other.numerator, denominator * other.denominator);
                result.Reduce();
                return result;
            }

            inline FractionBase operator*(int i) const {
                auto res = FractionBase(numerator * i, denominator);
                res.Reduce();
                return res;
            }

            FractionBase operator/(const FractionBase& other) const {
                if (other.numerator == T(0)) throw std::overflow_error("Division by zero");

                T factor = ((numerator < T(0) && other.numerator > T(0)) || (numerator > T(0) && other.numerator < T(0))) ? T(-1) : T(1);
                T newNumerator = factor * (numerator > T(0) ? numerator : -numerator) * other.denominator;
                T newDenominator = denominator * (other.numerator > T(0) ? other.numerator : -other.numerator);

                FractionBase result (newNumerator, newDenominator);
                result.Reduce();
                return result;
            }

            inline FractionBase operator/(int i) const {
                if (i == 0) throw std::overflow_error("Division by zero");

                T factor = (i < 0) ? T(-1) : T(1);
                T newNumerator = factor * numerator;
                T newDenominator = denominator * i;

                FractionBase result(newNumerator, newDenominator);
                result.Reduce();
                return result;
            }

            operator double() {
                return static_cast<double>(numerator) / static_cast<double>(denominator);
            }

            T GetNumerator() const {
                return numerator;
            }

            T GetDenominator() const {
                return denominator;
            }

            virtual double ToDouble() const override {
                return static_cast<double>(numerator) / static_cast<double>(denominator);
            }

            virtual std::string ToString() const override {
                // Do not write 0 to complicated
                if (numerator == T(0)) return "0";

                // Reduce
                FractionBase c = *this;
                c.Reduce();

                std::stringstream ss;
                if (c.denominator == T(1)) ss << c.numerator;
                else ss << c.numerator << "/" << c.denominator;
                return ss.str();
            }

            friend std::ostream& operator<<(std::ostream& os, const FractionBase& fraction) {
                os << fraction.ToString();
                return os;
            }

            virtual ScalarPointer Clone() const override {
                return std::move(ScalarPointer(new FractionBase(numerator, denominator)));
            }
        public:
            virtual void Serialize(std::ostream& os) const override {
                // Call parent
                AbstractScalar::Serialize(os);

                WriteBinary<T>(os, numerator);
                WriteBinary<T>(os, denominator);
            }

            static std::unique_ptr<AbstractScalar> Deserialize(std::istream& is) {
                // Call parent
                AbstractScalar::Deserialize(is);

                T numerator = ReadBinary<T>(is);
                T denominator = ReadBinary<T>(is);

                return std::move(std::unique_ptr<AbstractScalar>(new FractionBase(numerator, denominator)));
            }
        public:
            static FractionBase FromDouble(double f) {
                if (f < 0) return -FromDouble(-f);

                std::vector<int> values;

                int integer = static_cast<int>(f);
                double rest = f - static_cast<int>(integer);

                values.push_back(integer);
                int counter = 0;

                while (rest != 0 && rest > 1e-6) {
                    double x = 1.0/rest;

                    integer = static_cast<int>(x);
                    double diff = 1-(x - static_cast<double>(integer));
                    if (diff < 1e-6) ++integer;

                    rest = x - static_cast<double>(integer);

                    values.push_back(integer);
                }

                // Put together
                FractionBase result (T(values[values.size()-1]),T(1));

                for (int i=values.size()-2; i>=0; --i) {
                    // Invert the value
                    result = FractionBase(T(1), T(1)) / result;

                    // Add the current value
                    result += FractionBase(T(values[i]), T(1));
                }

                return result;
            }
        private:
            T numerator;
            T denominator;
        };

        typedef FractionBase<Construction::Common::BigNumber> Fraction;

    }
}