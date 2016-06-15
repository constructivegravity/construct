#pragma once

#include <sstream>
#include <random>

#include <common/datetime.hpp>

namespace Construction {
    namespace Common {

        class UUID {
        private:
            UUID() {
                GenerateTimeStamp();
            }
        public:
            UUID(const UUID& other) : low(other.low), mid(other.mid), node(other.node), category(other.category) { }
            UUID(UUID&& other) : low(std::move(other.low)), mid(std::move(other.mid)), node(std::move(other.node)), category(std::move(other.category)) { }
        public:
            inline bool operator==(const UUID& other) const {
                std::unique_lock<std::mutex> lock(mutex);
                return low == other.low && mid == other.mid && node == other.node && category == other.category;
            }

            inline bool operator!=(const UUID& other) const {
                std::unique_lock<std::mutex> lock(mutex);
                return low != other.low || mid != other.mid || node != other.node || category != other.category;
            }
        public:
            void GenerateTimeStamp() {
                std::unique_lock<std::mutex> lock(mutex);

                auto now = Datetime::Now();

                // Write year, month and day
                low  = (now.GetYear() & 65535) << 16;
                low |= static_cast<char>(now.GetMonth()) << 8;
                low |= static_cast<char>(now.GetDay());

                // Write hour, minute and second
                mid  = static_cast<char>(now.GetHour()) << 24;
                mid |= static_cast<char>(now.GetMinute()) << 16;
                mid |= static_cast<char>(now.GetSecond()) << 8;

                // Write secret
                std::random_device rd;
                std::default_random_engine generator(rd());
                std::uniform_int_distribution<char> distribution;
                mid |= distribution(generator) & 255;
            }
        public:
            inline Datetime GetDatetime() const {
                std::unique_lock<std::mutex> lock(mutex);

                return Datetime(
                    low >> 16,
                    (low >> 8) & 255,
                    low & 255,

                    mid >> 24,
                    (mid >> 16) & 255,
                    (mid >> 8) & 255
                );
            }

            inline int GetSecret() const {
                std::unique_lock<std::mutex> lock(mutex);

                return mid & 255;
            }

            inline int GetCategory() const {
                std::unique_lock<std::mutex> lock(mutex);

                return category;
            }

            inline long GetNode() const {
                std::unique_lock<std::mutex> lock(mutex);

                return node;
            }
        public:
            std::string ToString() const {
                std::unique_lock<std::mutex> lock(mutex);

                std::stringstream ss;

                ss << std::hex << low << mid << "-";
                ss << std::hex << category;
                ss << std::hex << node;

                return ss.str();
            }

            inline operator std::string() {
                std::unique_lock<std::mutex> lock(mutex);

                return ToString();
            }

            friend std::ostream& operator<<(std::ostream& os, const UUID& uuid) {
                os << uuid.ToString();
                return os;
            }
        public:
            static UUID Generate(unsigned category) {
                UUID result;

                // Assign category
                result.category = category;

                // Generate node
                std::random_device rd;
                std::default_random_engine generator(rd());
                std::uniform_int_distribution<long> distribution;
                result.node = distribution(generator);

                return result;
            }
        private:
            unsigned low, mid;
            long node;
            unsigned category;

            mutable std::mutex mutex;
        };

        /**
            \class Unique

            A simple policy to make the UUID generation simpler.
         */
        template<class T, unsigned category>
        class Unique {
        public:
            Unique() : uuid(UUID::Generate(category)) { }
            Unique(const UUID& uuid) : uuid(uuid) { }
        public:
            UUID GetUUID() const { return uuid; }
        public:
            bool operator==(const T& other) const { return uuid == other.uuid; }
            bool operator!=(const T& other) const { return uuid != other.uuid; }
        protected:
            UUID uuid;
        };

    }
}
