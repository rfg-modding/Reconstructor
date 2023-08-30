//License note: Originally from DashFaction, may have been modified: https://github.com/rafalh/dashfaction
//DashFaction is licensed under MPL2: https://github.com/rafalh/dashfaction/blob/c2e1b1cb42b637f5876268d91e6a4edb4f4b2cdc/LICENSE.txt
//Reconstructor is also licensed under MPL2 to keep things simple.

#pragma once
#include "common/patching/Traits.h"
#include "common/patching/Config.h"
#include <cstdint>
#include <subhook/subhook.h>
#include <stdexcept>

class FunHookImpl
{
protected:
    void* m_target_fun_ptr;
    void* m_hook_fun_ptr;
    subhook::Hook m_subhook;

    FunHookImpl(uintptr_t target_fun_addr, void* hook_fun_ptr)
    {
        m_target_fun_ptr = reinterpret_cast<void*>(target_fun_addr);
        m_hook_fun_ptr = hook_fun_ptr;
    }

public:
    //If useRelativeAddress is true m_target_fun_ptr will be treated as an address relative to the start of the rfg.exe module, which doesn't start at 0x0
    void Install(bool useRelativeAddress = true)
    {
        if(useRelativeAddress)
            SetAddr(CommonLib_ModuleBase + (uintptr_t)m_target_fun_ptr);

        m_subhook.Install(m_target_fun_ptr, m_hook_fun_ptr);
        if (!m_subhook.GetTrampoline())
            throw std::runtime_error("trampoline is null for 0x%p"); //Todo: add ptr value to error
    }

    void Remove()
    {
        m_subhook.Remove();
        //Todo: Add error check here
    }

    void SetAddr(uintptr_t target_fun_addr)
    {
        m_target_fun_ptr = reinterpret_cast<void*>(target_fun_addr);
    }
};

template<class T>
class FunHook;

template<class R, class... A>
class FunHook<R __cdecl(A...)> : public FunHookImpl
{
private:
    typedef R __cdecl FunType(A...);

public:
    FunHook(uintptr_t target_fun_addr, FunType* hook_fun_ptr) :
        FunHookImpl(target_fun_addr, reinterpret_cast<void*>(hook_fun_ptr))
    {}

    R CallTarget(A... a)
    {
        auto trampoline_ptr = reinterpret_cast<FunType*>(m_subhook.GetTrampoline());
        return trampoline_ptr(a...);
    }
};

template<class R, class... A>
class FunHook<R __fastcall(A...)> : public FunHookImpl
{
private:
    typedef R __fastcall FunType(A...);

public:
    FunHook(uintptr_t target_fun_addr, FunType* hook_fun_ptr) :
        FunHookImpl(target_fun_addr, reinterpret_cast<void*>(hook_fun_ptr))
    {}

    R CallTarget(A... a)
    {
        auto trampoline_ptr = reinterpret_cast<FunType*>(m_subhook.GetTrampoline());
        return trampoline_ptr(a...);
    }
};

template<class R, class... A>
class FunHook<R __stdcall(A...)> : public FunHookImpl
{
private:
    typedef R __stdcall FunType(A...);

public:
    FunHook(uintptr_t target_fun_addr, FunType* hook_fun_ptr) :
        FunHookImpl(target_fun_addr, reinterpret_cast<void*>(hook_fun_ptr))
    {}

    R CallTarget(A... a)
    {
        auto trampoline_ptr = reinterpret_cast<FunType*>(m_subhook.GetTrampoline());
        return trampoline_ptr(a...);
    }
};

#ifdef __cpp_deduction_guides
// deduction guide for lambda functions
template<class T>
FunHook(uintptr_t addr, T)->FunHook<typename function_traits<T>::f_type>;
#endif
