//License note: Originally from DashFaction, may have been modified: https://github.com/rafalh/dashfaction
//DashFaction is licensed under MPL2: https://github.com/rafalh/dashfaction/blob/c2e1b1cb42b637f5876268d91e6a4edb4f4b2cdc/LICENSE.txt
//RSL2 is also licensed under MPL2 to keep things simple.

#pragma once
#include "common/windows/WindowsWrapper.h"

class CodeBuffer
{
public:
    CodeBuffer(size_t len)
    {
        m_ptr = HeapAlloc(get_heap(), 0, len);
    }

    CodeBuffer(const CodeBuffer& other) = delete;

    ~CodeBuffer()
    {
        if (m_ptr) {
            HeapFree(get_heap(), 0, m_ptr);
        }
    }

    void* get() const
    {
        return m_ptr;
    }

    operator void*() const
    {
        return get();
    }

private:
    void* m_ptr;

    static HANDLE get_heap()
    {
        static auto heap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0);
        return heap;
    }
};
