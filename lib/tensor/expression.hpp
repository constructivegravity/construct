#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <iostream>

#include <common/serializable.hpp>
#include <common/printable.hpp>

using Construction::Common::Serializable;

namespace Construction {
	namespace Tensor {

		enum class ExpressionType {
			TENSOR = 1,
			SCALAR = 2,
			BOOLEAN = 3,
			INDICES = 4,

			SUBSTITUTION = 101,

			VOID_TYPE = 1001,
			STRING = 1002,

			UNKNOWN = 2001,
		};

		class AbstractExpression : public Common::Printable, public Serializable<AbstractExpression> {
		public:
			virtual ~AbstractExpression() { }
		public:
			virtual bool IsTensorExpression() const { return false; }
			virtual bool IsScalarExpression() const { return false; }
			virtual bool IsIndicesExpression() const { return false; }
			virtual bool IsBooleanExpression() const { return false; }
			virtual bool IsVoidExpression() const { return false; }
			virtual bool IsSubstitutionExpression() const { return false; }
			virtual bool IsStringExpression() const { return false; }
		public:
			virtual inline int GetColorCode() const { return 32; }
		public:
			virtual std::unique_ptr<AbstractExpression> Clone() const = 0;
		public:
			virtual std::string ToString() const override { return ""; }
		public:
			void Serialize(std::ostream& os) const override {
				// do nothing
            }

            static std::unique_ptr<AbstractExpression> Deserialize(std::istream& is) { return nullptr; }
		};

		typedef std::unique_ptr<AbstractExpression> 	ExpressionPointer;

		class VoidExpression : public AbstractExpression {
		public:
			VoidExpression() { }
		public:
			virtual ExpressionPointer Clone() const override { return std::move(ExpressionPointer(new VoidExpression())); }

			virtual bool IsVoidExpression() const override { return true; }
		public:
			virtual std::string ToString() const override {
				return "";
			}
		public:
			virtual void Serialize(std::ostream& os) const override {
				// do nothing
            }

            static std::unique_ptr<AbstractExpression> Deserialize(std::istream& is) { return ExpressionPointer(new VoidExpression()); }
		};

		class BoolExpression : public AbstractExpression {
		public:
			BoolExpression(bool value=false) : value(value) { }
		public:
			virtual ExpressionPointer Clone() const override { return std::move(ExpressionPointer(new BoolExpression(value))); }
		public:
			virtual std::string ToString() const override {
				return (value) ? "yes" : "no";
			}
		public:
			virtual int GetColorCode() const override { return (value) ? 32 : 31; }

			virtual bool IsBooleanExpression() const override { return true; }
		public:
			virtual void Serialize(std::ostream& os) const override {
				WriteBinary<char>(os, static_cast<char>(value));
            }

            static std::unique_ptr<AbstractExpression> Deserialize(std::istream& is) { 
            	bool value = static_cast<bool>(ReadBinary<char>(is));
            	return ExpressionPointer(new BoolExpression(value)); 
            }
		private:
			bool value;
		};

		class StringExpression : public AbstractExpression {
		public:
			StringExpression(const std::string& str) : value(str) { }
		public:
			virtual bool IsStringExpression() const override { return true; }
			virtual ExpressionPointer Clone() const override { return std::move(ExpressionPointer(new StringExpression(value))); }
		public:
			virtual std::string ToString() const override {
				return value;
			}
		public:
			virtual void Serialize(std::ostream& os) const override {
				WriteBinary<size_t>(os, static_cast<size_t>(value.size()));
				os << value;
			}

			static std::unique_ptr<AbstractExpression> Deserialize(std::istream& is) {
				size_t size = ReadBinary<size_t>(is);

				std::string value (size, ' ');
				is.read(&value[0], size);

				return ExpressionPointer(new StringExpression(value));
			}
		private:
			std::string value;
		};

		class Expression : public Common::Printable, public Serializable<Expression> {
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
			static Expression String(const std::string& value) { return Expression(ExpressionPointer(new StringExpression(value))); }
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
			bool IsScalar() const { return pointer->IsScalarExpression(); }
			bool IsTensor() const { return pointer->IsTensorExpression(); }
			bool IsBoolean() const { return pointer->IsBooleanExpression(); }
			bool IsVoid() const { return pointer->IsVoidExpression(); }
			bool IsSubstitution() const { return pointer->IsSubstitutionExpression(); }
			bool IsIndices() const { return pointer->IsIndicesExpression(); }
			bool IsString() const { return pointer->IsStringExpression(); }

			std::string TypeToString() const {
				switch (GetType()) {
					case ExpressionType::TENSOR: return "Tensor";
					case ExpressionType::SCALAR: return "Scalar";
					case ExpressionType::INDICES: return "Indices";
					case ExpressionType::BOOLEAN: return "Boolean";
					case ExpressionType::SUBSTITUTION: return "Substitution";
					case ExpressionType::VOID_TYPE: return "Void";
					case ExpressionType::STRING: return "String";
					default: return "Unknown";
				}
			}

			ExpressionType GetType() const {
				if (IsTensor()) return ExpressionType::TENSOR;
				else if (IsScalar()) return ExpressionType::SCALAR;
				else if (IsIndices()) return ExpressionType::INDICES;
				else if (IsBoolean()) return ExpressionType::BOOLEAN;
				else if (IsSubstitution()) return ExpressionType::SUBSTITUTION;
				else if (IsVoid()) return ExpressionType::VOID_TYPE;
				return ExpressionType::UNKNOWN;
			}
		public:
			virtual std::string ToString() const override { return pointer->ToString(); }
		public:
			virtual void Serialize(std::ostream& os) const override;
            static std::unique_ptr<Expression> Deserialize(std::istream& is);
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