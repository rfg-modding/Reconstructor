#pragma once
#include "common/Typedefs.h"
#include "common/patching/Config.h"
#include "common/patching/Offset.h"

class FunctionPointerImpl
{
protected:
    uintptr_t address_;
    void* ptr_ = nullptr;

    FunctionPointerImpl(uintptr_t address) : address_(address)
    {

    }

public:
    void Install(bool useRelativeAddress = true)
    {
        if (useRelativeAddress)
            address_ = CommonLib_ModuleBase + address_;

        ptr_ = reinterpret_cast<void*>(address_);
    }

    bool Initialized()
    {
        return ptr_ != nullptr;
    }
};

template<class T>
class FunctionPointer;

//Used to initialize function pointers to game functions with minimal boilerplate
template<class R, class... A>
class FunctionPointer<R __cdecl(A...)> : public FunctionPointerImpl
{
private:
    using FunPtrType = R(__cdecl*)(A...);

public:
    FunctionPointer(uintptr_t address) : FunctionPointerImpl(address)
    {
        Install();
    }

    R operator()(A... a)
    {
        FunPtrType func = reinterpret_cast<FunPtrType>(ptr_);
        return func(a...);
    }
};

template<class R, class... A>
class FunctionPointer<R __fastcall(A...)> : public FunctionPointerImpl
{
private:
    using FunPtrType = R(__fastcall*)(A...);

public:
    FunctionPointer(uintptr_t address) : FunctionPointerImpl(address)
    {
        Install();
    }

    R operator()(A... a)
    {
        FunPtrType func = reinterpret_cast<FunPtrType>(ptr_);
        return func(a...);
    }
};