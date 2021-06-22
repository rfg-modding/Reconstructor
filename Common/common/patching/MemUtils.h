//License note: Originally from DashFaction, may have been modified: https://github.com/rafalh/dashfaction
//DashFaction is licensed under MPL2: https://github.com/rafalh/dashfaction/blob/c2e1b1cb42b637f5876268d91e6a4edb4f4b2cdc/LICENSE.txt
//RSL2 is also licensed under MPL2 to keep things simple.

#pragma once
#include <cstring>
#include <stdint.h>

template<typename T>
struct TypeIdentity
{
    using type = T;
};

void WriteMem(unsigned addr, const void* data, unsigned size)
{
    DWORD old_protect;

    if (!VirtualProtect(reinterpret_cast<void*>(addr), size, PAGE_EXECUTE_READWRITE, &old_protect)) {
        printf("Warning: VirtualProtect failed: addr %x size %x error %lu\n", addr, size, GetLastError());
    }
    std::memcpy(reinterpret_cast<void*>(addr), data, size);
    VirtualProtect(reinterpret_cast<void*>(addr), size, old_protect, NULL);
}

void UnprotectMem(void* ptr, unsigned len)
{
    DWORD old_protect;
    if (!VirtualProtect(ptr, len, PAGE_EXECUTE_READWRITE, &old_protect)) {
        printf("Warning: VirtualProtect failed: addr %p size %x error %lu\n", ptr, len, GetLastError());
    }
}


template<typename T>
void WriteMem(uintptr_t addr, typename TypeIdentity<T>::type value)
{
    WriteMem(addr, &value, sizeof(value));
}

inline void WriteMem(unsigned addr, const void* data, unsigned size, unsigned num_repeat)
{
    while (num_repeat > 0) {
        WriteMem(addr, data, size);
        addr += size;
        --num_repeat;
    }
}

template<typename T>
inline void WriteMemPtr(unsigned addr, T* value)
{
    WriteMem(addr, &value, sizeof(value));
}

inline void WriteMemStr(unsigned addr, const char* str)
{
    WriteMem(addr, str, std::strlen(str) + 1);
}

template<typename T>
constexpr T& AddrAsRef(uintptr_t addr)
{
    return *reinterpret_cast<T*>(addr);
}

template<typename T>
T& StructFieldRef(void* struct_ptr, size_t offset)
{
    auto addr = reinterpret_cast<uintptr_t>(struct_ptr) + offset;
    return *reinterpret_cast<T*>(addr);
}

template<typename... A>
void *CallAddr(uintptr_t addr, A... args)
{
    return AddrAsRef<void*(A...)>(addr)(args...);
}
