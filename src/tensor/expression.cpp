#include <tensor/expression.hpp>
#include <tensor/scalar.hpp>
#include <tensor/tensor.hpp>
#include <tensor/substitution.hpp>

using Construction::Tensor::Expression;
using Construction::Tensor::Tensor;
using Construction::Tensor::Scalar;

void Expression::Serialize(std::ostream& os) const {
	// Write type
	WriteBinary<unsigned>(os, static_cast<unsigned>(pointer->GetType()));

	// Call the different serialization methods
	switch (pointer->GetType()) {
		case AbstractExpression::TENSOR:
			static_cast<Tensor*>(pointer.get())->Serialize(os);
			break;
		case AbstractExpression::SCALAR: 
			static_cast<Scalar*>(pointer.get())->Serialize(os);
			break;
		case AbstractExpression::BOOLEAN: 
			static_cast<BoolExpression*>(pointer.get())->Serialize(os);
			break;
		case AbstractExpression::SUBSTITUTION:
			static_cast<Substitution*>(pointer.get())->Serialize(os);
			break;
		case AbstractExpression::VOID_TYPE: 
			static_cast<VoidExpression*>(pointer.get())->Serialize(os);
			break;
	}
}

std::unique_ptr<Expression> Expression::Deserialize(std::istream& is) {
	// Read type
    AbstractExpression::Type type = static_cast<AbstractExpression::Type>(ReadBinary<unsigned>(is));

    std::unique_ptr<Expression> result;

    switch (type) {
    	case AbstractExpression::TENSOR:
    		result = std::unique_ptr<Expression>(new Expression(std::move(Tensor::Deserialize(is))));
			break;

		case AbstractExpression::SCALAR: 
			result = std::unique_ptr<Expression>(new Expression(std::move(Scalar::Deserialize(is))));
			break;

		case AbstractExpression::BOOLEAN: 
			result = std::unique_ptr<Expression>(new Expression(std::move(BoolExpression::Deserialize(is))));
			break;

		case AbstractExpression::SUBSTITUTION:
			result = std::unique_ptr<Expression>(new Expression(std::move(Substitution::Deserialize(is))));
			break;

		case AbstractExpression::VOID_TYPE: 
			result = std::unique_ptr<Expression>(new Expression(std::move(VoidExpression::Deserialize(is))));
			break;
    }

    return std::move(result);
}