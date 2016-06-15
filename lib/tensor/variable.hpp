#pragma once

#include <string>

#include <common/printable.hpp>
#include <common/uuid.hpp>

#include <tensor/scalar.hpp>

namespace Construction {
    namespace Tensor {

        using Common::Unique;
        using Common::Printable;

        /**
            \class Coefficient

            A coefficient is a
         */
        class Variable : public Scalar, Unique<Variable, 101>, Printable {
        public:
            Variable(const std::string& name) : name(name), Printable(name), Scalar(Scalar::VARIABLE) { }
            Variable(const std::string& name, const std::string& printed_text) : name(name), Printable(printed_text), Scalar(Scalar::VARIABLE) { }

            Variable(const Variable& other) : name(other.name), Printable(other.printed_text), Unique(other.uuid) { }
            //Variable(Variable&& other) : name(std::move(other.name)), Printable(std::move(other.printed_text)), uuid(std::move(other.uuid)) { }
        public:
            std::string GetName() const { return name; }
            void SetName(const std::string& name) { this->name = name; }
        public:
            virtual std::shared_ptr<Scalar> Clone() const override {
                return std::shared_ptr<Scalar>(new Variable(*this));
            }
        private:
            std::string name;
        };

    }
}
