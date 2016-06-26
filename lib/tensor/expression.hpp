#pragma once

#include <common/printable.hpp>

namespace Construction {
	namespace Tensor {

		class AbstractExpression : public Common::Printable {
		public:
			enum Type {
				TENSOR = 1,
				SCALAR = 2,
				BOOLEAN = 3,

				TENSOR_LIST = 4,

				VOID_TYPE = 1001,
			};
		public:
			AbstractExpression(Type type) : type(type) { }
		public:
			bool IsTensor() const { return type == TENSOR; }
			bool IsScalar() const { return type == SCALAR; }
			bool IsTensorList() const { return type == TENSOR_LIST; }
			bool IsBoolean() const { return type == BOOLEAN; }
			bool IsVoid() const { return type == VOID_TYPE; }

			Type GetType() const { return type; }
		public:
			virtual std::unique_ptr<AbstractExpression> Clone() const = 0;
		public:
			virtual std::string ToString() const { return ""; }
		private:
			Type type;
		};

		typedef std::unique_ptr<AbstractExpression> 	ExpressionPointer;

		class VoidExpression : public AbstractExpression {
		public:
			VoidExpression() : AbstractExpression(VOID_TYPE) { }
		public:
			virtual ExpressionPointer Clone() const { return std::move(ExpressionPointer(new VoidExpression())); }
		public:
			virtual std::string ToString() const {
				return "";
			}
		};

		class BoolExpression : public AbstractExpression {
		public:
			BoolExpression(bool value=false) : AbstractExpression(BOOLEAN), value(value) { }
		public:
			virtual ExpressionPointer Clone() const { return std::move(ExpressionPointer(new BoolExpression(value))); }
		public:
			virtual std::string ToString() const {
				return (value) ? "yes" : "no";
			}
		private:
			bool value;
		};

		class Expression : public Common::Printable {
		public:
			Expression() : pointer(ExpressionPointer(new VoidExpression())) { }

			Expression(const Expression& other) : pointer(std::move(other.pointer->Clone())) { }
			Expression(Expression&& other) : pointer(std::move(other.pointer)) { }

			Expression(const AbstractExpression& expr) : pointer(expr.Clone()) { }
			Expression(AbstractExpression&& expr) : pointer(std::move(ExpressionPointer(&expr))) { }
		private:
			Expression(ExpressionPointer pointer) : pointer(std::move(pointer)) { }
		public:
			static Expression Void() { return Expression(); }

			static Expression Boolean(bool value) { return Expression(ExpressionPointer(new BoolExpression(value))); }
			static Expression True() { return Expression(ExpressionPointer(new BoolExpression(true))); }
			static Expression False() { return Expression(ExpressionPointer(new BoolExpression(false))); }
		public:
			Expression& operator=(const Expression& other) {
				pointer = std::move(other.pointer->Clone());
				return *this;
			}

			Expression& operator=(Expression&& other) {
				pointer = std::move(other.pointer);
				return *this;
			}
		public:
			bool IsScalar() const { return pointer->IsScalar(); }
			bool IsTensor() const { return pointer->IsTensor(); }
			bool IsBoolean() const { return pointer->IsBoolean(); }
			bool IsVoid() const { return pointer->IsVoid(); }
			bool IsTensorList() const { return pointer->IsTensorList(); }

			AbstractExpression::Type GetType() const { return pointer->GetType(); }
		public:
			virtual std::string ToString() const override { return pointer->ToString(); }
		public:
			template<class T>
			T As() { return *static_cast<T*>(pointer.get()); }

			template<class T>
			const T* As() const { return static_cast<const T*>(pointer.get()); }
		private:
			ExpressionPointer pointer;
		};

	}
}