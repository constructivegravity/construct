#pragma once

#include <tensor/scalar.hpp>
#include <tensor/tensor.hpp>

namespace Construction {
	namespace Tensor {

		class Tensor;

		class Substitution : public AbstractExpression {
		public:
			Substitution() : AbstractExpression(AbstractExpression::SUBSTITUTION, 36) { }

			Substitution(const Scalar& variable, const Scalar& other) : AbstractExpression(AbstractExpression::SUBSTITUTION, 36) {
				substitutions.push_back({variable,other}); 
			}
		public:
			void Insert(const Scalar& variable, const Scalar& expression) {
				substitutions.push_back({variable, expression});
			}
		public:
			inline Scalar operator()(const Scalar& scalar) const {
				Scalar result = scalar;
				for (auto& s : substitutions) {
					result = std::move(result.Substitute(s.first, s.second));
				}
				return result;
			}

			inline Tensor operator()(const Tensor& tensor) const {
				return tensor.SubstituteVariables(substitutions);
			}
		public:
			virtual ExpressionPointer Clone() const override {
				return ExpressionPointer(new Substitution(*this));
			}

			virtual std::string ToString() const override {
				std::stringstream ss;

				for (auto& s : substitutions) {
					ss << s.first << " = " << s.second << std::endl;
				}

				return ss.str();
			}
		private:
			std::vector< std::pair<Scalar, Scalar> > substitutions;
		};

	}
}