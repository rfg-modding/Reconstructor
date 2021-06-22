//License note: Originally from DashFaction, may have been modified: https://github.com/rafalh/dashfaction
//DashFaction is licensed under MPL2: https://github.com/rafalh/dashfaction/blob/c2e1b1cb42b637f5876268d91e6a4edb4f4b2cdc/LICENSE.txt
//RSL2 is also licensed under MPL2 to keep things simple.

#pragma once

// For generic types that are functors, delegate to its 'operator()'
template<typename T>
struct function_traits : public function_traits<decltype(&T::operator())>
{
};

// for pointers to member function
template<typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
{
    // enum { arity = sizeof...(Args) };
    typedef ReturnType f_type(Args...);
};

// for pointers to member function
template<typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...)>
{
    typedef ReturnType f_type(Args...);
};

// for function pointers
template<typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)>
{
    typedef ReturnType f_type(Args...);
};
