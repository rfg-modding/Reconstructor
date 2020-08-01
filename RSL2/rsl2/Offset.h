#pragma once
#include "misc/GlobalState.h"

template<class T>
T OffsetPtr(unsigned long Offset)
{
    static RSL2_GlobalState* globalState = GetGlobalState();
    if (globalState->ModuleBase == 0)
    {
        globalState->ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
    }
    return reinterpret_cast<T>(globalState->ModuleBase + Offset);
}