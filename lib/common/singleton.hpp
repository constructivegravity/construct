#pragma once

template<typename Class>
class Singleton {
public:
    static Class* Instance() {
        if (!m_instance)
            m_instance = new Class();
        return m_instance;
    }

    virtual ~Singleton() {
        m_instance = 0;
    }
private:
    static Class* m_instance;
protected:
    Singleton() {}
};

template<typename Class>
Class* Singleton<Class>::m_instance = 0;