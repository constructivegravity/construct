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
        class Variable : public AbstractScalar, public Construction::Common::Printable {
        public:
            Variable(const std::string& name) : name(name), Printable(name), AbstractScalar(AbstractScalar::VARIABLE) { }
            Variable(const std::string& name, const std::string& printed_text) : name(name), Printable(printed_text), AbstractScalar(AbstractScalar::VARIABLE) { }

            Variable(const Variable& other) : AbstractScalar(AbstractScalar::VARIABLE), name(other.name), Printable(other.printed_text) { }
            //Variable(Variable&& other) : name(std::move(other.name)), Printable(std::move(other.printed_text)) { }

            virtual ~Variable() = default;
        public:
            std::string GetName() const { return name; }
            void SetName(const std::string& name) { this->name = name; }
        public:
            virtual ScalarPointer Clone() const override {
                return ScalarPointer(new Variable(*this));
            }
        public:
            virtual std::string ToString() const override { 
                return printed_text;
            }

            virtual void Serialize(std::ostream& os) const override {
                AbstractScalar::Serialize(os);

                WriteBinary<size_t>(os, static_cast<size_t>(name.size()));
                os.write(reinterpret_cast<const char*>(name.c_str()), name.size());
            }

            static std::unique_ptr<AbstractScalar> Deserialize(std::istream& is) {
                AbstractScalar::Deserialize(is);

                size_t size = ReadBinary<size_t>(is);

                std::string name (size, ' ');
                is.read(&name[0], size);

                return ScalarPointer(new Variable(name));
            }
        private:
            std::string name;
        };

    }
}
