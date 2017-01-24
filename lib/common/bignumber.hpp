#pragma once

#include <string>
#include <cmath>
#include <sstream>
#include <iostream>
#include <random>

#include <common/serializable.hpp>

namespace Construction {
    namespace Common {

        /**
            \class BigNumber


         */
        class BigNumber : public Serializable<BigNumber> {
        public:
            BigNumber() { }

            BigNumber(unsigned i) {
                values.push_back(i);
            }

            BigNumber(int i) {
                values.push_back(static_cast<unsigned>(i));
            }

            /*BigNumber(long i) {
                unsigned times = sizeof(long) / sizeof(int);
                for (int i=0; i<times; ++i) {
                    values.push_back(static_cast<unsigned>((i >> times*sizeof(i)) & 4294967295));
                }
            }

            BigNumber(long long i) {
                unsigned times = sizeof(long long) / sizeof(int);
                for (int i=0; i<times; ++i) {
                    values.push_back(static_cast<unsigned>((i >> times*sizeof(i)) & 4294967295));
                }
            }*/

            BigNumber(const std::string& string) {
                *this = std::move(FromString(string));
            }

            BigNumber(const char* str) {
                *this = std::move(FromString(std::string(str)));
            }

            BigNumber(const BigNumber& other) : values(other.values) { }
            BigNumber(BigNumber&& other) : values(std::move(other.values)) { }
        public:
            BigNumber&operator=(const BigNumber& other) {
                values = other.values;
                return *this;
            }

            BigNumber&operator=(BigNumber&& other) {
                values = std::move(other.values);
                return *this;
            }

            BigNumber& operator=(unsigned i) {
                values = { i };
                return *this;
            }

            BigNumber& operator=(int i) {
                values = { static_cast<unsigned>(i) };
                return *this;
            }

            /*BigNumber& operator=(long long i) {
                values.clear();

                unsigned times = sizeof(long long) / sizeof(int);
                for (int i=0; i<times; ++i) {
                    values.push_back(static_cast<unsigned>((i >> times*sizeof(i)) & 4294967295));
                }

                return *this;
            }*/

            BigNumber& operator=(const std::string& string) {
                values = FromString(string).values;
                return *this;
            }
        public:
            static BigNumber FromString(std::string string) {
                // Handle negative numbers
                if (string.size() > 0 && string[0] == '-') return FromString(string.substr(1)).Negated();

                BigNumber result;

                // Handle exponents
                {
                    auto pose = string.find("e");
                    auto posE = string.find("E");

                    auto pos = (pose != std::string::npos) ? pose : posE;

                    if (pos != std::string::npos) {
                        auto base = FromString(string.substr(0, pos));
                        auto exp = FromString(string.substr(pos+1));

                        if (exp.IsNegative()) return BigNumber(0);

                        return base * BigNumber::Pow(BigNumber(10), exp);
                    }
                }

                char pos = 0;
                unsigned current = 0;

                while (string != "0" && string != "") {
                    bool value;
                    string = DivideStringByTwo(string, &value);

                    // Set the value
                    if (value) {
                        current = current | (1 << pos);
                    }

                    pos++;
                    if (pos >= sizeof(unsigned)*8) {
                        pos = 0;
                        result.values.push_back(current);
                        current = 0;
                    }
                }

                // Add remaining value
                if (current != 0) result.values.push_back(current);

                // If the result would be negative, add a zero
                if (result.IsNegative()) result.values.push_back(0);

                return result;
            }

            static BigNumber FromBinaryString(const std::string& data) {
                std::stringstream ss (data);

                BigNumber result;

                unsigned value;
                while (ss >> value) {
                    result.values.push_back(value);
                }

                return result;
            }
        public:
            bool IsNegative() const {
                if (values.size() == 0) return false;
                return (values.back() & (1 << 31)) == (1 << 31);
            }

            bool IsPositive() const {
                if (values.size() == 0) return true;
                return (values.back() & (1 << 31)) != (1 << 31);
            }

            inline size_t Size() const {
                return values.size();
            }

            size_t GetEffectiveSize() const {
                size_t result = values.size();
                unsigned n = (IsPositive()) ? 0 : 4294967295;

                while (result > 0 && (values[result-1] == n)) {
                    --result;
                }

                return result;
            }
        protected:
            void Extend(unsigned length=1) {
                // Already contains that many or more bytes
                if (values.size() >= length) return;

                unsigned empty = IsNegative() ? 4294967295 : 0;
                while (values.size() < length) values.push_back(empty);
            }

            void Shrink(unsigned min=1) {
                bool wasNegative = IsNegative();
                int n = (wasNegative) ? 4294967295 : 0;
                while (values.size() > min) {
                    if (values[values.size()-1] != n) return;
                    values.pop_back();
                }

                // Check if the sign is still correct
                if (wasNegative && !IsNegative()) values.push_back(4294967295);
                else if (!wasNegative && IsNegative()) values.push_back(0);
            }
        public:
            void Negate() {
                // Do not waste time on zeros
                if (values.size() == 0) return;

                for (int i=values.size()-1; i>=0; --i) {
                    values[i] = ~values[i];
                }

                // Increase the bottom element
                ++(*this);
            }

            BigNumber Negated() const {
                auto copy = *this;
                copy.Negate();
                return copy;
            }
        public:
            bool GetBitAt(size_t pos) const {
                size_t byte = pos / 32;
                pos = pos % 32;

                if (byte >= values.size()) {
                    return (IsNegative()) ? 1 : 0;
                }

                return (values[byte] & 1<<pos) == (1<<pos);
            }
        private:
            static std::string AddTwoStrings(const std::string& one, const std::string& two) {
                // Helper function to extend a decimal string to the given length
                auto extend = [](const std::string& value, size_t length) {
                    if (value.size() >= length) return value;

                    std::string output = value;

                    while (output.size() < length) {
                        output = "0" + output;
                    }

                    return output;
                };

                // Extend the strings to the same length
                auto length = std::max(one.size(), two.size());
                auto first = extend(one, length);
                auto second = extend(two, length);

                // Prepare output
                std::string result(length, ' ');

                char carry=0;
                for (int i=length-1; i>=0; --i) {
                    char numberA = first[i] - '0';
                    char numberB = second[i] - '0';

                    char added = numberA + numberB + carry;
                    if (added > 9) {
                        carry = 1;
                        result[i] = ('0' + (added-10));
                    } else {
                        carry = 0;
                        result[i] = ('0' + added);
                    }
                }

                if (carry > 0) {
                    return "1" + result;
                }

                return result;
            }

            static std::string SubtractTwoStrings(const std::string& one, const std::string& two) {
                // Helper function to extend a decimal string to the given length
                auto extend = [](const std::string& value, size_t length) {
                    if (value.size() >= length) return value;

                    std::string output = value;

                    while (output.size() < length) {
                        output = "0" + output;
                    }

                    return output;
                };

                // Extend the strings to the same length
                auto length = std::max(one.size(), two.size());
                auto first = extend(one, length);
                auto second = extend(two, length);

                // Prepare output
                std::string result(length, ' ');

                char carry=0;
                for (int i=length-1; i>=0; --i) {
                    char numberA = first[i] - '0';
                    char numberB = second[i] - '0';

                    char added = numberA - numberB - carry;
                    if (added < 0) {
                        carry = 1;
                        result[i] = ('0' + (added+10));
                    } else {
                        carry = 0;
                        result[i] = ('0' + added);
                    }
                }

                if (carry > 0) {
                    return "-" + result;
                }

                return result;
            }

            static std::string DivideStringByTwo(const std::string& string, bool* rest = nullptr) {
                char carry = 0;
                std::string result = "";

                for (int i=0; i<string.size(); ++i) {
                    char value = (string[i]-'0') + (carry * 10);
                    char divided = value / 2;

                    // Set the value
                    result += std::string(1, divided + '0');

                    carry = (value & 1) == 1;
                }

                // Set the rest, if one is interested in it
                if (rest) *rest = (carry != 0);

                // If the first letter is a zero, omit that
                if (result.size() > 1 && result[0] == '0') return result.substr(1);

                return result;
            }
        public:
            std::string ToBinaryString(bool padding=false) const {
                std::string output = "";

                for (int i=values.size()-1; i>=0; --i) {
                    // Add to the string
                    for (int j=sizeof(values[i])*8-1; j>=0; --j) {
                        output += ((values[i] & (1 << j)) == (1 << j)) ? "1" : "0";
                    }

                    if (padding) output += " ";
                }

                return output;
            }

            std::string ToDecimalString() const {
                // Get the number of power of 2
                std::string pows = "1";
                std::string result = "0";

                if (IsNegative()) return "-" + Negated().ToDecimalString();

                for (int i=0; i<values.size() * 32; ++i) {
                    bool value = GetBitAt(i);

                    // If this byte is set, add the current power of 2 to the result
                    if (value) {
                        result = AddTwoStrings(result, pows);
                    }

                    // Go to the next power of two
                    pows = AddTwoStrings(pows, pows);
                }

                // Return the result
                return result;
            }

            std::string ToHexString(bool padding=false) const {
                std::string output;

                for (int i=values.size(); i>=0; --i) {
                    std::stringstream ss;
                    ss << std::hex << values[i];

                    if (padding) ss << " ";

                    output += ss.str();
                }

                return output;
            }

            inline std::string ToString() const {
                return ToDecimalString();
            }
        public:
            virtual void Serialize(std::ostream& os) const {
                WriteBinary<size_t>(os, values.size());

                for (auto& byte : values) {
                    WriteBinary<unsigned>(os, byte);
                }
            }

            static std::unique_ptr<BigNumber> Deserialize(std::istream& is) {
                std::unique_ptr<BigNumber> result (new BigNumber());

                size_t nums = ReadBinary<size_t>(is);

                for (int i=0; i<nums; ++i) {
                    result->values.push_back(ReadBinary<unsigned>(is));
                }

                return std::move(result);
            }
        public:
            void ShiftLeft() {
                bool overflow=false;
                bool neg = IsNegative();

                for (auto& byte : values) {
                    bool overflowNew = (byte & 1<<31) == 1<<31;
                    byte = byte << 1;
                    if (overflow) ++byte;
                    overflow = overflowNew;
                }

                if (neg && (values[values.size()-1] & 1<<31) == 0) {
                    values.push_back(4294967295);
                } else if (!neg && (values[values.size()-1] & 1<<31) == 1<<31) {
                    values.push_back(0);
                }
            }
        public:
            BigNumber operator+=(const BigNumber& other) {
                // Extend the number
                Extend(other.values.size());
                auto copy = other;
                copy.Extend(values.size());

                // Check if this was negative before
                bool isNegative = IsNegative();

                unsigned carry=0;
                for (int i=0; i<values.size(); ++i) {
                    unsigned tmp = copy.values[i] + carry;
                    unsigned result = values[i] + tmp;

                    carry = (values[i] > result) | (copy.values[i] > tmp);
                    values[i] = result;
                }

                // If this was not negative before, but now is, add a zero
                if ((!isNegative && !other.IsNegative()) && IsNegative()) values.push_back(0);
                else if ((isNegative && other.IsNegative()) && !IsNegative()) values.push_back(4294967295);

                return *this;
            }

            inline BigNumber operator+(const BigNumber& other) const {
                auto result = *this;
                result += other;
                return result;
            }

            inline BigNumber operator++() {
                *this += BigNumber(1);
                return *this;
            }

            inline BigNumber operator++(int) {
                BigNumber copy = *this;
                *this += BigNumber(1);
                return copy;
            }

            inline BigNumber operator-() const {
                return Negated();
            }

            inline BigNumber& operator-=(const BigNumber& other) {
                *this += (-other);
                return *this;
            }

            inline BigNumber operator-(const BigNumber& other) const {
                return *this + (-other);
            }

            BigNumber& operator*=(const BigNumber& other) {
                BigNumber copy = other;
                BigNumber left = *this;

                // only true if this is a negative number xor the other one is one
                bool resultNegative = (!IsNegative() != !other.IsNegative());

                // make both positive
                if (IsNegative()) Negate();
                if (copy.IsNegative()) copy.Negate();

                BigNumber result = 0;

                for (int i=0; i <= copy.Size()*sizeof(unsigned)*8-1; ++i) {
                    // If this zero is one
                    if (copy.GetBitAt(i)) {
                        result += left;
                    }

                    // Shift this to the left
                    left.ShiftLeft();
                }

                // Set the value
                values = result.values;

                // If the result is negative, add a zero
                if (IsNegative()) { values.push_back(0); }

                // if the result has to be negative, negate
                if (resultNegative) Negate();

                // Return result
                return *this;
            }

            inline BigNumber operator*(const BigNumber& other) const {
                auto result = *this;
                result *= other;
                return result;
            }

            static inline BigNumber Divide(const BigNumber& a, const BigNumber& b, BigNumber* rest = nullptr) {
                if (b == BigNumber(0)) throw std::overflow_error("Division by zero");

                if (b.IsNegative()) {
                    BigNumber restCopy;
                    auto q = Divide(a, -b, &restCopy);
                    if (rest) *rest = restCopy;

                    return -q;
                }

                if (a.IsNegative()) {
                    BigNumber restCopy;
                    auto q = Divide(-a, b, &restCopy);

                    if (restCopy == BigNumber(0)) {
                        if (rest) *rest = BigNumber(0);
                        return -q;
                    }

                    if (rest) *rest = b - restCopy;
                    return -(q+BigNumber(1));
                }

                BigNumber q (0);
                BigNumber r = a;

                while (r >= b) {
                    q = q + BigNumber(1);
                    r = r - b;
                }

                if (rest) *rest = r;

                return q;
            }

            inline BigNumber& operator/=(const BigNumber& other) {
                *this = Divide(*this, other);
                return *this;
            }

            inline BigNumber operator/(const BigNumber& other) const {
                return Divide(*this, other);
            }

            inline BigNumber operator%(const BigNumber& other) const {
                BigNumber result;
                Divide(*this, other, &result);
                return result;
            }
        public:
            bool operator==(const BigNumber& other) const {
                // Determine the effective size
                auto effectiveSizeA = GetEffectiveSize();
                auto effectiveSizeB = other.GetEffectiveSize();

                // If they are of different effective size, return false
                if (effectiveSizeA != effectiveSizeB) return false;

                for (int i=0; i<effectiveSizeA; ++i) {
                    if (values[i] != other.values[i]) return false;
                }

                return true;
            }

            bool operator!=(const BigNumber& other) const {
                // Determine the effective size
                auto effectiveSizeA = GetEffectiveSize();
                auto effectiveSizeB = other.GetEffectiveSize();

                // If they are of different effective size, return false
                if (effectiveSizeA != effectiveSizeB) return true;

                for (int i=0; i<effectiveSizeA; ++i) {
                    if (values[i] != other.values[i]) return true;
                }

                return false;
            }

            inline bool operator<(const BigNumber& other) const {
                // If they have different signs, easy peasy
                if (IsNegative() && !other.IsNegative()) return true;
                else if (!IsNegative() && other.IsNegative()) return false;

                // Determine the effective size
                auto effectiveSizeA = GetEffectiveSize();
                auto effectiveSizeB = other.GetEffectiveSize();

                if (effectiveSizeA < effectiveSizeB) return !IsNegative();
                else if (effectiveSizeA > effectiveSizeB) return IsNegative();

                // Compare all bytes
                for (int i=effectiveSizeA-1; i>=0; --i) {
                    if (values[i] < other.values[i]) return !IsNegative();
                    else if (values[i] > other.values[i]) return IsNegative();
                }

                return false;
            }

            inline bool operator<=(const BigNumber& other) const {
                // If they have different signs, easy peasy
                if (IsNegative() && !other.IsNegative()) return true;
                else if (!IsNegative() && other.IsNegative()) return false;

                // Determine the effective size
                auto effectiveSizeA = GetEffectiveSize();
                auto effectiveSizeB = other.GetEffectiveSize();

                if (effectiveSizeA < effectiveSizeB) return !IsNegative();
                else if (effectiveSizeA > effectiveSizeB) return IsNegative();

                // Compare all bytes
                for (int i=effectiveSizeA-1; i>=0; --i) {
                    if (values[i] < other.values[i]) return !IsNegative();
                    else if (values[i] > other.values[i]) return IsNegative();
                }

                return true;
            }

            inline bool operator>(const BigNumber& other) const {
                return other < *this;
            }

            inline bool operator>=(const BigNumber& other) const {
                return other <= *this;
            }
        public:
            bool IsOdd() const {
                if (values.size() == 0) return false;
                return values[0] % 2 == 1;
            }

            bool IsEven() const {
                if (values.size() == 0) return true;
                return values[0] % 2 == 0;
            }

            /*bool IsPrime(int accuracy=5) const {
                auto nmo = *this -1;

                // Select r,d such that nmo = Pow(2,r) * d

                for (int i=0; i<accuracy; ++i) {
                    // Pick random integer a in the range [2, n-2]
                    auto a = GetRandomNumber(2, *this - 2);

                    auto x = Pow(a, d) % *this;

                    if (x == 1 || x == nmo) {
                        continue;
                    }

                    bool continueOuter = false;
                    for (int j=0; j<r; ++j) {
                        x = (x*x) % *this;
                        if (x == 1) return false;
                        else if (x == nmo) {
                            continueOuter = true;
                            break;
                        }
                    }

                    // Continue the outer loop
                    if (continueOuter) continue;

                    return false;
                }

                return true;
            }*/
        public:
            static BigNumber Pow(const BigNumber& base, const BigNumber& exp) {
                BigNumber result = base;
                for (BigNumber i=1; i<exp; ++i) {
                    result *= base;
                }
                return result;
            }
        public:
            static BigNumber GetRandomNumber(const BigNumber& lower, const BigNumber& higher) {
                // Get the number of bytes to generate
                auto size = std::max(lower.Size(), higher.Size());

                static std::random_device r;
                static std::default_random_engine engine(r());
                static std::uniform_int_distribution<unsigned> dist;

                BigNumber result;
                do {
                    // Clear the number
                    result.values.clear();

                    // Generate random unsigneds
                    for (int i=0; i<size; ++i) {
                        result.values.push_back(dist(engine));
                    }
                } while (result < lower || result > higher);

                return result;
            }
        public:
            friend std::ostream& operator<<(std::ostream& os, const BigNumber& number) {
                os << number.ToString();
                return os;
            }

            /*operator int() const {
                assert(values.size() == 1);
                return values[0];
            }*/

            operator double() const {
                assert(values.size() == 1);
                return static_cast<double>(values[0]);
            }
        private:
            std::vector<unsigned> values;
        };

    }
}