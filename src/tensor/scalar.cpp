#include <tensor/scalar.hpp>
#include <tensor/fraction.hpp>
#include <tensor/variable.hpp>

#include <iostream>
#include <sstream>

using namespace Construction::Tensor;

Scalar::Scalar() : AbstractExpression(SCALAR), pointer(ScalarPointer(new Tensor::Fraction(0))) { }
Scalar::Scalar(double v) : AbstractExpression(SCALAR), pointer(ScalarPointer(new FloatingPointScalar(v))) { }
Scalar::Scalar(int v) : AbstractExpression(SCALAR), pointer(ScalarPointer(new Tensor::Fraction(v))) { }
Scalar::Scalar(int numerator, unsigned denominator) : AbstractExpression(SCALAR), pointer(ScalarPointer(new Tensor::Fraction(numerator, denominator))) { }
Scalar::Scalar(const std::string& name) : AbstractExpression(SCALAR), pointer(ScalarPointer(new Tensor::Variable(name))) { }
Scalar::Scalar(const std::string& name, const std::string& printed_text) : AbstractExpression(SCALAR), pointer(ScalarPointer(new Tensor::Variable(name, printed_text))) { }
Scalar::Scalar(const std::string& name, unsigned id) : AbstractExpression(SCALAR) {
    std::stringstream ss;
    ss << name << "_" << id;
    pointer = ScalarPointer(new Tensor::Variable(ss.str()));   
}

Scalar& Scalar::operator=(double d) {
    pointer = ScalarPointer(new FloatingPointScalar(d));
    return *this;
}

ScalarPointer AbstractScalar::Add(const AbstractScalar& one, const AbstractScalar& other) {
    // Clone the pointers
    ScalarPointer first = one.Clone();
    ScalarPointer second = other.Clone();

    // Do not add zero
    if (one.IsNumeric() && one.ToDouble() == 0) return std::move(second);
    if (other.IsNumeric() && other.ToDouble() == 0) return std::move(first);

    // Do some simplification black magic
    if (one.IsFraction() && other.IsFraction()) {
        return ScalarPointer(new Fraction((*static_cast<Fraction*>(first.get())) + (*static_cast<Fraction*>(second.get()))));
    }

    if ( one.IsNumeric() && other.IsNumeric() && (other.IsFloatingPoint() || one.IsFloatingPoint())) {
        return ScalarPointer(new FloatingPointScalar(one.ToDouble() + other.ToDouble()));
    }

    // From now on, we can assume that one of the summands contains a variable

    // If both are the same, multiply them
    if (Scalar(first->Clone()) == Scalar(second->Clone())) {
        return std::move(Multiply(Fraction(2), one));
    }

    // If one is the negative of the other, return zero
    if (Scalar(first->Clone()) == -Scalar(second->Clone())) {
        return ScalarPointer(new Fraction());
    }

    // Simplify linear variable expressions
    /*if (one.IsMultiplied() && static_cast<MultipliedScalar*>(first.get())->A->IsNumeric() && static_cast<MultipliedScalar*>(first.get())->B->IsVariable()) {
        Variable* v = static_cast<Variable*>(static_cast<MultipliedScalar*>(first.get())->B.get());
        AbstractScalar* factor1 = static_cast<Variable*>(static_cast<MultipliedScalar*>(first.get())->A.get());

        if (other.IsVariable() && static_cast<Variable*>(second.get())->ToString() == v->ToString()) {
            return std::move(Multiply(
                *Add(*static_cast<MultipliedScalar*>(first.get())->A->Clone(), Fraction(1)),

                // Variable
                *static_cast<MultipliedScalar*>(first.get())->B->Clone()
            ));
        }

        if (other.IsMultiplied() && static_cast<MultipliedScalar*>(second.get())->B->IsVariable() && static_cast<MultipliedScalar*>(second.get())->B->ToString() == v->ToString()) {
            return std::move(Multiply(
                *Add(
                    *static_cast<MultipliedScalar*>(first.get())->A->Clone(), 
                    *static_cast<MultipliedScalar*>(second.get())->A->Clone(),
                ),

                // Variable
                *static_cast<MultipliedScalar*>(first.get())->B->Clone()
            ));
        }
    }*/

    // If the first one is a sum, try to simplify
    if ((one.IsAdded() && other.IsNumeric())) {
        return ScalarPointer(new AddedScalar(
            std::move(Add(*second, *static_cast<AddedScalar*>(first.get())->A->Clone())),
            std::move(static_cast<AddedScalar*>(first.get())->B->Clone())
        ));
    }

    // If the second one is a sum, try to simplify
    if ((other.IsAdded() && one.IsNumeric())) {
        std::unique_ptr<AddedScalar> added(&dynamic_cast<AddedScalar&>(*second.get()));

        return ScalarPointer(new AddedScalar(
            std::move(Add(*first, *static_cast<AddedScalar*>(second.get())->A->Clone())),
            std::move(static_cast<AddedScalar*>(second.get())->B->Clone())
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

    // If one is 0, just return zero. Good for killing variables...
    if ((one.IsNumeric() && one.ToDouble() == 0) || (other.IsNumeric() && other.ToDouble() == 0)) {
        return ScalarPointer(new Fraction());
    }

    // Do some simplification black magic
    if (one.IsFraction() && other.IsFraction()) {
        return ScalarPointer(new Fraction((*static_cast<Fraction*>(first.get())) * (*static_cast<Fraction*>(second.get()))));
    }

    // Both numerics, but one of those is a floating point
    if ( one.IsNumeric() && other.IsNumeric() && (other.IsFloatingPoint() || one.IsFloatingPoint())) {
        return ScalarPointer(new FloatingPointScalar(one.ToDouble() * other.ToDouble()));
    }

    // If one of the components is 1, just return the other
    if (one.IsNumeric() && one.ToDouble() == 1) return std::move(second);
    if (other.IsNumeric() && other.ToDouble() == 1) return std::move(first);

    // If the first one is a product, try to simplify
    if ((one.IsMultiplied() && other.IsNumeric())) {
        auto t = Multiply(*second, *static_cast<MultipliedScalar*>(first.get())->A->Clone());

        // Syntact sugar, get rid of 1 and 0
        if (t->IsNumeric()) {
            double d = t->ToDouble();
            if (d == 0) return ScalarPointer(new Fraction());
            else if (d == 1) return ScalarPointer(std::move(static_cast<MultipliedScalar*>(first.get())->B->Clone()));
        }

        return ScalarPointer(new MultipliedScalar(
            std::move(t),
            std::move(static_cast<MultipliedScalar*>(first.get())->B->Clone())
        ));
    }

    // If the second one is a product, try to simplify
    if ((other.IsMultiplied() && one.IsNumeric())) {
        auto t = Multiply(*first, *static_cast<MultipliedScalar*>(second.get())->A->Clone());

        // Syntact sugar, get rid of 1 and 0
        if (t->IsNumeric()) {
            double d = t->ToDouble();
            if (d == 0) return ScalarPointer(new Fraction());
            else if (d == 1) return ScalarPointer(std::move(static_cast<MultipliedScalar*>(second.get())->B->Clone()));
        }

        return ScalarPointer(new MultipliedScalar(
            std::move(t),
            std::move(static_cast<MultipliedScalar*>(second.get())->B->Clone())
        ));
    }

    // If the other is a number, put on the left
    // This works and is not redundant, since we already know that one is NOT a number
    // As a result, we can always assume that numbers are on the left and use this
    // to simplify a tree in the expression
    if (other.IsFraction() || other.IsFloatingPoint()) {
        return ScalarPointer(new MultipliedScalar(std::move(second), std::move(first)));
    }

    return ScalarPointer(new MultipliedScalar(std::move(first), std::move(second)));
}

ScalarPointer AbstractScalar::Negate(const AbstractScalar& one) {
    return std::move(Multiply(Fraction(-1), one));
}

ScalarPointer AbstractScalar::Subtract(const AbstractScalar& one, const AbstractScalar& other) {
    return std::move(Add(one, *Negate(other)));
}

bool Scalar::operator==(const Scalar& other) const {
    if (IsNumeric() && other.IsNumeric()) return ToDouble() == other.ToDouble();
    if ((IsNumeric() && other.IsVariable()) || (IsVariable() && other.IsNumeric())) return false;
    if (IsVariable() && other.IsVariable()) return static_cast<class Variable*>(pointer.get())->GetName() == static_cast<class Variable*>(other.pointer.get())->GetName();

    if (IsAdded() && other.IsAdded()) {
        Scalar firstA = Scalar(static_cast<AddedScalar*>(pointer.get())->GetFirst()->Clone());
        Scalar firstB = Scalar(static_cast<AddedScalar*>(pointer.get())->GetSecond()->Clone());

        Scalar secondA = Scalar(static_cast<AddedScalar*>(other.pointer.get())->GetFirst()->Clone());
        Scalar secondB = Scalar(static_cast<AddedScalar*>(other.pointer.get())->GetSecond()->Clone()); 

        return ((firstA == secondA && firstB == secondB) || (firstA == secondB && firstB == secondA));                   
    }

    if (IsMultiplied() && other.IsMultiplied()) {
        Scalar firstA = Scalar(static_cast<MultipliedScalar*>(pointer.get())->GetFirst()->Clone());
        Scalar firstB = Scalar(static_cast<MultipliedScalar*>(pointer.get())->GetSecond()->Clone());

        Scalar secondA = Scalar(static_cast<MultipliedScalar*>(other.pointer.get())->GetFirst()->Clone());
        Scalar secondB = Scalar(static_cast<MultipliedScalar*>(other.pointer.get())->GetSecond()->Clone()); 

        return ((firstA == secondA && firstB == secondB) || (firstA == secondB && firstB == secondA));                   
    }            

    return false;
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
