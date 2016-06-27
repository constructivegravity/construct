#include <tensor/scalar.hpp>
#include <tensor/fraction.hpp>
#include <tensor/variable.hpp>

#include <sstream>

using namespace Construction::Tensor;

Scalar::Scalar() : AbstractExpression(SCALAR), pointer(ScalarPointer(new Fraction(0))) { }
Scalar::Scalar(double v) : AbstractExpression(SCALAR), pointer(ScalarPointer(new FloatingPointScalar(v))) { }
Scalar::Scalar(int v) : AbstractExpression(SCALAR), pointer(ScalarPointer(new Fraction(v))) { }
Scalar::Scalar(int numerator, unsigned denominator) : AbstractExpression(SCALAR), pointer(ScalarPointer(new Fraction(numerator, denominator))) { }
Scalar::Scalar(const std::string& name) : AbstractExpression(SCALAR), pointer(ScalarPointer(new Variable(name))) { }
Scalar::Scalar(const std::string& name, const std::string& printed_text) : AbstractExpression(SCALAR), pointer(ScalarPointer(new Variable(name, printed_text))) { }
Scalar::Scalar(const std::string& name, unsigned id) : AbstractExpression(SCALAR) {
    std::stringstream ss;
    ss << name << "_" << id;
    pointer = ScalarPointer(new Variable(ss.str()));   
}

Scalar& Scalar::operator=(double d) {
    pointer = ScalarPointer(new FloatingPointScalar(d));
    return *this;
}

ScalarPointer AbstractScalar::Add(const AbstractScalar& one, const AbstractScalar& other) {
    // Clone the pointers
    ScalarPointer first = one.Clone();
    ScalarPointer second = other.Clone();

    // Do some simplification black magic
    if (one.IsFraction() && other.IsFraction()) {
        std::unique_ptr<Fraction> a(&dynamic_cast<Fraction&>(*first.get()));
        std::unique_ptr<Fraction> b(&dynamic_cast<Fraction&>(*second.get()));

        ScalarPointer result (new Fraction(*a + *b));
        return std::move(result);
    }

    if (other.IsFloatingPoint() || one.IsFloatingPoint()) {
        return ScalarPointer(new FloatingPointScalar(one.ToDouble() + other.ToDouble()));
    }

    // If the first one is a sum, try to simplify
    if ((one.IsAdded() && other.IsNumeric())) {
        std::unique_ptr<AddedScalar> added(&dynamic_cast<AddedScalar&>(*first.get()));

        return ScalarPointer(new AddedScalar(
            std::move(Add(*second, *added->A->Clone())),
            std::move(added->B->Clone())
        ));
    }

    // Do not add zero
    if (one.IsNumeric() && one.ToDouble() == 0) return std::move(second);
    if (other.IsNumeric() && other.ToDouble() == 0) return std::move(first);

    // If the second one is a sum, try to simplify
    if ((other.IsAdded() && one.IsNumeric())) {
        std::unique_ptr<AddedScalar> added(&dynamic_cast<AddedScalar&>(*second.get()));

        return ScalarPointer(new AddedScalar(
            std::move(Add(*first, *added->A->Clone())),
            std::move(added->B->Clone())
        ));
    }

    // If the other is a number, put on the left
    // This works and is not redundant, since we already know that one is NOT a number
    // As a result, we can always assume that numbers are on the left and use this
    // to simplify a tree in the expression
    if (other.IsFraction() || other.IsFloatingPoint()) {
        return ScalarPointer(new AddedScalar(std::move(first), std::move(second)));
    }

    return ScalarPointer(new AddedScalar(std::move(first), std::move(second)));
}

ScalarPointer AbstractScalar::Multiply(const AbstractScalar& one, const AbstractScalar& other) {
    // Clone the pointers
    ScalarPointer first = one.Clone();
    ScalarPointer second = other.Clone();

    // Do some simplification black magic
    if (one.IsFraction() && other.IsFraction()) {
        std::unique_ptr<Fraction> a(&dynamic_cast<Fraction&>(*first.get()));
        std::unique_ptr<Fraction> b(&dynamic_cast<Fraction&>(*second.get()));
        
        ScalarPointer result (new Fraction((*a) * (*b)));
        return std::move(result);
    }

    if (other.IsFloatingPoint() || one.IsFloatingPoint()) {
        return ScalarPointer(new FloatingPointScalar(one.ToDouble() * other.ToDouble()));
    }

    // If one is 0, just return zero. Good for killing variables...
    if ((one.IsNumeric() && one.ToDouble() == 0) || (other.IsNumeric() && other.ToDouble() == 0)) {
        return ScalarPointer(new Fraction());
    }

    // If one of the components is 1, just return the other
    if (one.IsNumeric() && one.ToDouble() == 1) return std::move(second);
    if (other.IsNumeric() && other.ToDouble() == 1) return std::move(first);

    // If the first one is a product, try to simplify
    if ((one.IsMultiplied() && other.IsNumeric())) {
        std::unique_ptr<MultipliedScalar> multiplied(&dynamic_cast<MultipliedScalar&>(*first.get()));

        return ScalarPointer(new MultipliedScalar(
            std::move(Multiply(*second, *multiplied->A->Clone())),
            std::move(multiplied->B->Clone())
        ));
    }

    // If the second one is a product, try to simplify
    if ((other.IsAdded() && one.IsNumeric())) {
        std::unique_ptr<MultipliedScalar> multiplied(&dynamic_cast<MultipliedScalar&>(*second.get()));

        return ScalarPointer(new MultipliedScalar(
            std::move(Multiply(*first, *multiplied->A->Clone())),
            std::move(multiplied->B->Clone())
        ));
    }

    // If the other is a number, put on the left
    // This works and is not redundant, since we already know that one is NOT a number
    // As a result, we can always assume that numbers are on the left and use this
    // to simplify a tree in the expression
    if (other.IsFraction() || other.IsFloatingPoint()) {
        return ScalarPointer(new MultipliedScalar(std::move(first), std::move(second)));
    }

    return ScalarPointer(new MultipliedScalar(std::move(first), std::move(second)));
}

ScalarPointer AbstractScalar::Negate(const AbstractScalar& one) {
    ScalarPointer first = one.Clone();

    // Do some simplification black magic
    if (one.IsFraction()) {
        std::unique_ptr<Fraction> a(&dynamic_cast<Fraction&>(*first.get()));

        return ScalarPointer(new Fraction(-(*a)));
    }

    if (one.IsFloatingPoint()) {
        return ScalarPointer(new FloatingPointScalar(-
            one.ToDouble()));
    }

    return ScalarPointer(new MultipliedScalar(ScalarPointer(new Fraction(-1)), std::move(one.Clone())));
}

ScalarPointer AbstractScalar::Subtract(const AbstractScalar& one, const AbstractScalar& other) {
    return std::move(Add(one, *Negate(other)));
}

std::vector<ScalarPointer> AbstractScalar::GetVariables() const {
    std::vector<ScalarPointer> result;

    std::function<void(const AbstractScalar*)> fn = [&](const AbstractScalar* scalar) {
        switch (scalar->GetType()) {
            case VARIABLE:
                result.push_back(scalar->Clone());
                break;
            case ADDED:
                fn(static_cast<const AddedScalar*>(scalar)->A.get());
                fn(static_cast<const AddedScalar*>(scalar)->B.get());
                break;
            case MULTIPLIED:
                fn(static_cast<const MultipliedScalar*>(scalar)->A.get());
                fn(static_cast<const MultipliedScalar*>(scalar)->B.get());
                break;
            default:
                ;
        }
    };

    fn(this);

    return result;
}

bool AbstractScalar::HasVariables() const {
    bool hasVariables = false;
    std::function<void(const AbstractScalar*)> fn = [&](const AbstractScalar* scalar) -> void {
        switch (scalar->GetType()) {
            case VARIABLE:
                hasVariables = true;
                break;
            case ADDED:
                fn(static_cast<const AddedScalar*>(scalar)->A.get());
                fn(static_cast<const AddedScalar*>(scalar)->B.get());
                break;
            case MULTIPLIED:
                fn(static_cast<const MultipliedScalar*>(scalar)->A.get());
                fn(static_cast<const MultipliedScalar*>(scalar)->B.get());
                break;
            default:
                ;
        }
    };

    fn(this);
    return hasVariables;
}

void AbstractScalar::Serialize(std::ostream& os) const {

}

void Scalar::Serialize(std::ostream& os) const {

}

//virtual void Scalar::Replace(const Variable& variable, const Scalar& scalar) {
    // Recursive helper function to go through all the scalars
    /*std::function<void()> fn = [&](const Scalar* sc) {
        switch (sc->GetType()) {
            case VARIABLE:
                if (*sc == variable) {

                }
                break;
            case ADDED:
                fn(static_cast<const AddedScalar*>(sc)->GetFirst().get());
                fn(static_cast<const AddedScalar*>(sc)->GetSecond().get());
                break;
            case MULTIPLIED:
                fn(static_cast<const MultipliedScalar*>(sc)->GetFirst().get());
                fn(static_cast<const MultipliedScalar*>(sc)->GetSecond().get());
                break;
            default:
                ;
        }
    }

    fn(this);*/
//}
