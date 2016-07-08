#pragma once

#include <memory>

#include <common/printable.hpp>

namespace Construction {
	namespace Tensor {

		class AbstractExpression : public Common::Printable {
		public:
			enum Type {
				TENSOR = 1,
				SCALAR = 2,
				BOOLEAN = 3,

				SUBSTITUTION = 101,

				VOID_TYPE = 1001,
			};
		public:
			AbstractExpression(Type type) : type(type), colorCode(32) { }
			AbstractExpression(Type type, int colorCode) : type(type), colorCode(colorCode) { }
		public:
			bool IsTensor() const { return type == TENSOR; }
			bool IsScalar() const { return type == SCALAR; }
			bool IsBoolean() const { return type == BOOLEAN; }
			bool IsVoid() const { return type == VOID_TYPE; }
			bool IsSubstitution() const { return type == SUBSTITUTION; }

			Type GetType() const { return type; }
		public:
			virtual inline int GetColorCode() const { return colorCode; }
		public:
			virtual std::unique_ptr<AbstractExpression> Clone() const = 0;
		public:
			virtual std::string ToString() const { return ""; }
		private:
			Type type;
			int colorCode;
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
		public:
			virtual int GetColorCode() const override { return (value) ? 32 : 31; }
		private:
			bool value;
		};

		class Expression : public Common::Printable {
		public:
			Expression() : pointer(ExpressionPointer(new VoidExpression())) { }

			Expression(const Expression& other) : pointer(std::move(other.pointer->Clone())) { }
			Expression(Expression&& other) : pointer(std::move(other.pointer)) { }

			Expression(const AbstractExpression& expr) : pointer(expr.Clone()) { }
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
			inline int GetColorCode() const { return pointer->GetColorCode(); }
		public:
			bool IsScalar() const { return pointer->IsScalar(); }
			bool IsTensor() const { return pointer->IsTensor(); }
			bool IsBoolean() const { return pointer->IsBoolean(); }
			bool IsVoid() const { return pointer->IsVoid(); }
			bool IsSubstitution() const { return pointer->IsSubstitution(); }

			std::string TypeToString() const {
				switch (pointer->GetType()) {
					case AbstractExpression::TENSOR: return "Tensor";
					case AbstractExpression::SCALAR: return "Scalar";
					case AbstractExpression::BOOLEAN: return "Boolean";
					case AbstractExpression::SUBSTITUTION: return "Substitution";
					case AbstractExpression::VOID_TYPE: return "Void";
				}
				return "Unknown";
			}

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