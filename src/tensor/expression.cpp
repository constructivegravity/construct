#include <tensor/expression.hpp>
#include <tensor/scalar.hpp>
#include <tensor/tensor.hpp>
#include <tensor/substitution.hpp>

using Construction::Tensor::Expression;
using Construction::Tensor::Tensor;
using Construction::Tensor::Scalar;

void Expression::Serialize(std::ostream& os) const {
	// Write type
	WriteBinary<unsigned>(os, static_cast<unsigned>(GetType()));

	// Call the different serialization methods
	switch (GetType()) {
		case ExpressionType::TENSOR:
			static_cast<Tensor*>(pointer.get())->Serialize(os);
			break;
		case ExpressionType::SCALAR:
			static_cast<Scalar*>(pointer.get())->Serialize(os);
			break;
		case ExpressionType::BOOLEAN:
			static_cast<BoolExpression*>(pointer.get())->Serialize(os);
			break;
		case ExpressionType::INDICES:
			static_cast<Indices*>(pointer.get())->Serialize(os);
			break;
		case ExpressionType::SUBSTITUTION:
			static_cast<Substitution*>(pointer.get())->Serialize(os);
			break;
		case ExpressionType::VOID_TYPE:
			static_cast<VoidExpression*>(pointer.get())->Serialize(os);
			break;
		case ExpressionType::STRING:
			static_cast<StringExpression*>(pointer.get())->Serialize(os);
			break;
		case ExpressionType::UNKNOWN:
			break;
	}
}

std::unique_ptr<Expression> Expression::Deserialize(std::istream& is) {
	// Read type
    ExpressionType type = static_cast<ExpressionType>(ReadBinary<unsigned>(is));

    std::unique_ptr<Expression> result;

    switch (type) {
    	case ExpressionType::TENSOR:
    		result = std::unique_ptr<Expression>(new Expression(std::move(Tensor::Deserialize(is))));
			break;

		case ExpressionType::SCALAR:
			result = std::unique_ptr<Expression>(new Expression(std::move(Scalar::Deserialize(is))));
			break;

		case ExpressionType::INDICES:
			result = std::unique_ptr<Expression>(new Expression(std::move(Indices::Deserialize(is))));
			break;

		case ExpressionType::BOOLEAN:
			result = std::unique_ptr<Expression>(new Expression(std::move(BoolExpression::Deserialize(is))));
			break;

		case ExpressionType::SUBSTITUTION:
			result = std::unique_ptr<Expression>(new Expression(std::move(Substitution::Deserialize(is))));
			break;

		case ExpressionType::VOID_TYPE:
			result = std::unique_ptr<Expression>(new Expression(std::move(VoidExpression::Deserialize(is))));
			break;

		case ExpressionType::STRING:
			result = std::unique_ptr<Expression>(new Expression(std::move(StringExpression::Deserialize(is))));
			break;

		case ExpressionType::UNKNOWN:
			result = nullptr;
			break;
    }

    return std::move(result);
}