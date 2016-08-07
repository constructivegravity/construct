#pragma once

#include <common/error.hpp>
#include <tensor/scalar.hpp>
#include <tensor/tensor.hpp>

namespace Construction {
	namespace Tensor {

		class Tensor;

        class InvalidSubstitutionException : public Exception {
        public:
            InvalidSubstitutionException() : Exception("The substitution is invalid") { }
        };

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
		public:
			virtual void Serialize(std::ostream& os) const override {
				// Write size
				WriteBinary<size_t>(os, substitutions.size());

				// Write the pairs
				for (auto& substitution : substitutions) {
					substitution.first.Serialize(os);
					substitution.second.Serialize(os);
				}
			}

			static std::unique_ptr<AbstractExpression> Deserialize(std::istream& is) {
				// Read size
				size_t size = ReadBinary<size_t>(is);

				std::unique_ptr<Substitution> result (new Substitution());

				// Iterate over the entries
				for (int i=0; i<size; i++) {
					auto lhs = Scalar::Deserialize(is);
					if (!lhs) return nullptr;

					auto rhs = Scalar::Deserialize(is);
					if (!rhs) return nullptr;

					result->Insert(*static_cast<Scalar*>(lhs.get()), *static_cast<Scalar*>(rhs.get()));
				}

				return std::move(result);
			}
		private:
			std::vector< std::pair<Scalar, Scalar> > substitutions;
		};

	}
}
