#pragma once
#include "common/Typedefs.h"
#include "common/patching/Config.h"

template<class T>
T OffsetPtr(unsigned long Offset)
{
    return reinterpret_cast<T>(CommonLib_ModuleBase + Offset);
}