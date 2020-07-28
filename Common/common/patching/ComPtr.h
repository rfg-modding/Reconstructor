//License note: Originally from DashFaction, may have been modified: https://github.com/rafalh/dashfaction
//It's license is MPL2 (same as RSL2) to keep things simple: https://github.com/rafalh/dashfaction/blob/c2e1b1cb42b637f5876268d91e6a4edb4f4b2cdc/LICENSE.txt

#pragma once

template<typename T>
class ComPtr
{
private:
    T *m_ptr = nullptr;

public:
    ComPtr()
    {}

    ComPtr(const ComPtr& other) :
        m_ptr(other.m_ptr)
    {
        if (m_ptr)
            m_ptr->AddRef();
    }

    ~ComPtr()
    {
        release();
    }

    ComPtr& operator=(const ComPtr& other)
    {
        release();
        m_ptr = other.m_ptr;
        if (m_ptr)
            m_ptr->AddRef();
        return *this;
    }

    T** operator&()
    {
        release();
        return &m_ptr;
    }

    operator T*()
    {
        return m_ptr;
    }

    T* operator->()
    {
        return m_ptr;
    }

    void release()
    {
        if (m_ptr)
            m_ptr->Release();
        m_ptr = nullptr;
    }
};
