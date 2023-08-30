#include "reconstructor/functions/FunctionsInternal.h"
#include "RFGR_Types/rfg/Animation.h"
#include "common/string/String.h"
#include "misc/GlobalState.h"
#include <filesystem>
#include <stdexcept>
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif
#include <vector>

//Note: These are currently only for debugging purposes for the anim_rig array size increase patch in \patching\Patches.h

//int __cdecl anim_rig_find_index(const char *filename) //0x0088AAC0
FunHook<int __cdecl(const char* filename)> anim_rig_find_index_hook
{
    0x0088AAC0,
    [](const char* filename) -> int
    {
        int result = anim_rig_find_index_hook.CallTarget(filename);
        if (String::EqualIgnoreCase(string(filename), string("walker_civilian.rig_pc")))
        {
            int debug0 = 0;
            
            Reconstructor_GlobalState* globalState = GetGlobalState();
            std::vector<anim_rig*> rigs = {};
            for (u32 i = 0; i < 500; i++)
            {
                anim_rig* rig = &globalState->CustomAnimRigArray[i];
                rigs.push_back(rig);
            }
            
            int debug1 = 0;
        }
        if (result == -1)
        {
            printf("%s\n", filename);
            int a = 0;
            
            Reconstructor_GlobalState* globalState = GetGlobalState();
            std::vector<anim_rig*> rigs = {};
            for (u32 i = 0; i < 500; i++)
            {
                anim_rig* rig = &globalState->CustomAnimRigArray[i];
                rigs.push_back(rig);
            }
            
            int b = 0;
        }
        return result;
    }
};

//anim_rig* __cdecl animlib_get_loaded_rig(const char *filename) //0x00890930
FunHook<anim_rig* __cdecl(const char* name)> animlib_get_loaded_rig_hook
{
    0x00890930,
    [](const char* name) -> anim_rig*
    {
        int index = rfg::Functions.anim_rig_find_index(name);
        if (index > -1)
        {
            return &(GetGlobalState()->CustomAnimRigArray[index]);
        }
#ifdef DEBUG_BUILD
        else
        {
            printf("animlib_get_loaded_rig() failed to find '%s'\n", name); //TODO: Add proper logging
        }
#endif
        return nullptr;
    }
};