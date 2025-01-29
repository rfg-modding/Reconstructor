#pragma once
#include "common/patching/FunHook.h"
#include "common/Typedefs.h"
#include "reconstructor/misc/GlobalState.h"
#include "common/BuildConfig.h"
#include "rfg/keen/Memory.h"
#include <memoryapi.h>
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#include <span>
#endif

//0x008E5DC0; const keen::MemoryLayoutDefinition* __cdecl keen::getMemoryLayoutDefinition()
FunHook<keen::MemoryLayoutDefinition*  __cdecl()> keen_getMemoryLayoutDefinition_hook
{
    0x008E5DC0,
    []() -> keen::MemoryLayoutDefinition*
    {
        keen::MemoryLayoutDefinition* memoryLayout = keen_getMemoryLayoutDefinition_hook.CallTarget();

        //Get span on the blocks for easier debugging. VS only shows the first item in the array when you reference the array with a pointer.
        std::span<keen::MemoryBlockDefinition> blocks = std::span<keen::MemoryBlockDefinition>(memoryLayout->pBlockDefinitions, memoryLayout->blockCount);

        DWORD old_protect;
        const size_t blocksSize = memoryLayout->blockCount * sizeof(keen::MemoryBlockDefinition);
        BOOL res = VirtualProtect(reinterpret_cast<void*>(memoryLayout->pBlockDefinitions), blocksSize, PAGE_EXECUTE_READWRITE, &old_protect);

        //Increase size of "RFG_MempoolProvider" which seems to be the backing for many of rfgs other mempools and/or allocators. 
        //Was getting log errors about it running out of space after making some pools larger for mods.
        //Vanilla size is 1,146,000,000
        keen::MemoryBlockDefinition& rfgMempoolProviderBlock = memoryLayout->pBlockDefinitions[2];
        rfgMempoolProviderBlock.size = 1346000000;

        VirtualProtect(reinterpret_cast<void*>(memoryLayout->pBlockDefinitions), blocksSize, old_protect, NULL);

        return memoryLayout;
    }
};