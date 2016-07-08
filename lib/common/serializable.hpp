#pragma once

#include <memory>

#include <common/error.hpp>

namespace Construction {
    namespace Common {

        class WrongFormatException : public Exception {
        public:
            WrongFormatException() : Exception("Wrong format. Cannot serialize/deserialize") { }
        };

        template<typename T>
        class Serializable {
        public:
            /**
                Serialize the content into a ostream
             */
            virtual void Serialize(std::ostream& os) const = 0;

            /**
                Deserialize
             */
            static std::unique_ptr<T> Deserialize(std::istream& is) {
                return std::move(std::unique_ptr<T>(nullptr));
            }
        public:
            template<typename S>
            void WriteBinary(std::ostream& os, S data) const {
                //os << "(" << data << ")";
                os.write(reinterpret_cast<const char*>(&data), sizeof(data));
                //os.write(reinterpret_cast<const char*>(&data), sizeof(data));
            }

            template<typename S>
            static S ReadBinary(std::istream& is) {
                S data;
                is.read(reinterpret_cast<char*>(&data), sizeof(data));
                return data;
            }
        };

    }
}