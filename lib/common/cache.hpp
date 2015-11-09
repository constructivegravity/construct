#pragma once

#include <common/singleton.hpp>

namespace Construction {
    namespace Common {

        template<typename K, typename V>
        class Cache : public Singleton<Cache> {
        public:
            Cache() { }
        public:
            bool Contains(const K& key) const {
                auto it = data.find(key);
                if (it == data.end()) return false;
                return true;
            }

            V Get(const K& key) const {
                auto it = data.find(key);
                if (it == data.end()) return V();
                return it->second;
            }

            V& Get(const K& key) {
                return data[key];
            }

            void Set(const K& key, const V& value) {
                data[key] = value;
            }
        private:
            std::map<K, V> data;
        };

    }
}