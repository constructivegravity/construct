#pragma once

#include <mutex>

template<typename Class>
class Singleton {
public:
    static Class* Instance() {
        std::unique_lock<std::mutex> lock(mutex);

        if (!m_instance)
            m_instance = new Class();
        return m_instance;
    }

    virtual ~Singleton() {
        m_instance = 0;
    }
private:
    static std::mutex mutex;
    static Class* m_instance;
protected:
    Singleton() {}
};

template<typename Class>
Class* Singleton<Class>::m_instance = 0;

template<typename Class>
std::mutex Singleton<Class>::mutex;