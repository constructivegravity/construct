#include <tensor/scalar.hpp>
#include <tensor/fraction.hpp>
#include <tensor/variable.hpp>

using namespace Construction::Tensor;

std::shared_ptr<Scalar> Add(const Scalar& one, const Scalar& other) {
    // Clone the pointers
    std::shared_ptr<Scalar> first = one.Clone();
    std::shared_ptr<Scalar> second = other.Clone();

    // Do some simplification black magic
    if (one.IsFraction() && other.IsFraction()) {
        return std::move(((*std::dynamic_pointer_cast<Fraction>(first)) + (*std::dynamic_pointer_cast<Fraction>(second))).Clone());
    }

    if (other.IsFloatingPoint() || one.IsFloatingPoint()) {
        return std::shared_ptr<Scalar>(new FloatingPointScalar(one.ToDouble() + other.ToDouble()));
    }

    // If the other is a number, put on the left
    // This works and is not redundant, since we already know that one is NOT a number
    // As a result, we can always assume that numbers are on the left and use this
    // to simplify a tree in the expression
    if (other.IsFraction() || other.IsFloatingPoint()) {
        return std::shared_ptr<Scalar>(new AddedScalar(std::move(first), std::move(second)));
    }

    return std::shared_ptr<Scalar>(new AddedScalar(std::move(first), std::move(second)));
}

std::shared_ptr<Scalar> Multiply(const Scalar& one, const Scalar& other) {
    // Clone the pointers
    std::shared_ptr<Scalar> first = one.Clone();
    std::shared_ptr<Scalar> second = other.Clone();

    // Do some simplification black magic
    if (one.IsFraction() && other.IsFraction()) {
        return std::move(((*std::dynamic_pointer_cast<Fraction>(first)) * (*std::dynamic_pointer_cast<Fraction>(second))).Clone());
    }

    if (other.IsFloatingPoint() || one.IsFloatingPoint()) {
        return std::shared_ptr<Scalar>(new FloatingPointScalar(one.ToDouble() * other.ToDouble()));
    }

    // If the other is a number, put on the left
    // This works and is not redundant, since we already know that one is NOT a number
    // As a result, we can always assume that numbers are on the left and use this
    // to simplify a tree in the expression
    if (other.IsFraction() || other.IsFloatingPoint()) {
        return std::shared_ptr<Scalar>(new MultipliedScalar(std::move(first), std::move(second)));
    }

    return std::shared_ptr<Scalar>(new MultipliedScalar(std::move(first), std::move(second)));
}

std::shared_ptr<Scalar> Negate(const Scalar& one) {
    std::shared_ptr<Scalar> first = one.Clone();

    // Do some simplification black magic
    if (one.IsFraction()) {
        return std::shared_ptr<Scalar>(new Fraction(-(*std::dynamic_pointer_cast<Fraction>(first))));
    }

    if (one.IsFloatingPoint()) {
        return std::shared_ptr<Scalar>(new FloatingPointScalar(-
            one.ToDouble()));
    }

    return std::shared_ptr<Scalar>(new MultipliedScalar(std::make_shared<Fraction>(-1), std::move(one.Clone())));
}

/*Scalar& Scalar::operator+(const Scalar& other) const {
    // Do some simplification black magic
    if (type == FRACTION && other.type == FRACTION) {
        return (reinterpret_cast<const Fraction>(*this) + reinterpret_cast<const Fraction>(other));
    }

    if (other.type == FLOATING_POINT || type == FLOATING_POINT) {
        return FloatingPointScalar(ToDouble() + other.ToDouble());
    }

    // If this is a variable then potentially the other scalar is a
    // number, thus invert the order since addition commutes anyway
    if (type == VARIABLE) {
        return AddedScalar(std::move(other.Clone()), std::move(Clone()));
    }

    return AddedScalar(std::move(Clone()), std::move(other.Clone()));
}

Scalar& Scalar::operator*(const Scalar& other) const {
    // Do some simplification black magic
    if (type == FRACTION && other.type == FRACTION) {
        return (reinterpret_cast<const Fraction>(*this) * reinterpret_cast<const Fraction>(other));
    }

    if (other.type == FLOATING_POINT || type == FLOATING_POINT) {
        return FloatingPointScalar(ToDouble() * other.ToDouble());
    }

    return MultipliedScalar(std::move(Clone()), std::move(other.Clone()));
}

Scalar& Scalar::operator*(double c) const {
    // Do some simplification black magic
    if (type == FRACTION || type == FLOATING_POINT) {
        return FloatingPointScalar(ToDouble() * other.ToDouble());
    }

    return MultipliedScalar(std::move(Clone()), std::move(other.Clone()));
}

friend inline Scalar& Scalar::operator*(double c, const Tensor& other) {
    return other*c;
}

Scalar& Scalar::operator-() const {
    // Do some simplification black magic
    if (type == FRACTION) {
        return (-reinterpret_cast<Fraction>(*this));
    }

    if (type == FLOATING_POINT) {
        return FloatingPointScalar(-static_cast<double>(*this));
    }

    return MultipliedScalar(std::make_shared<Fraction>(-1), std::move(Clone()));
}

// Let the compiler do the nasty stuff
inline Scalar& Scalar::operator-(const Scalar& other) const {
    return (*this) + (-other);
}
*/
