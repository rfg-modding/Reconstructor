//License note: Originally from DashFaction, may have been modified: https://github.com/rafalh/dashfaction
//DashFaction is licensed under MPL2: https://github.com/rafalh/dashfaction/blob/c2e1b1cb42b637f5876268d91e6a4edb4f4b2cdc/LICENSE.txt
//RSL2 is also licensed under MPL2 to keep things simple.

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
