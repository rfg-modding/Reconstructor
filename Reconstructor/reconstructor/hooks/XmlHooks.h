#pragma once
#include "common/patching/FunHook.h"
#include "common/Typedefs.h"
#include "common/filesystem/File.h"
#include "RFGR_Types/rfg/xml/Xml.h"
#include "RFGR_Types/rfg/Memory.h"
#include "reconstructor/misc/GlobalState.h"
#include "reconstructor/functions/Functions.h"
#include "reconstructor/util/Util.h"
#include "common/patching/Offset.h"
#include "common/BuildConfig.h"
#include "reconstructor/functions/FunctionsInternal.h"
#include <filesystem>
#ifdef COMPILE_IN_PROFILER
#include "tracy/Tracy.hpp"
#endif

//xml_element* __cdecl xml_parse(const char* filename, mempool_base* dest) //0x001CD2F0
FunHook<xml_element*(const char* filename, mempool_base* dest)> xml_parse_hook
{
    //Hooks xml_parse so we can inject our own xml files from outside of the vpps
    0x001CD2F0,
    [](const char* filename, mempool_base* dest) -> xml_element*
    {
        Reconstructor_GlobalState* globalState = GetGlobalState();
        rfg::RfgFunctions* functions = GetRfgFunctions();
        if (!globalState || !functions)
            return nullptr;

        //Load override if one exists
#ifdef DEBUG_BUILD
        string overridePath = "D:/GOG/Games/Red Faction Guerrilla Re-Mars-tered/Reconstructor/Overrides/" + string(filename);
#elif defined DEBUG_BUILD_OPTIMIZED
        string overridePath = "D:/GOG/Games/Red Faction Guerrilla Re-Mars-tered/Reconstructor/Overrides/" + string(filename);
#else
        string overridePath = "./Reconstructor/Overrides/" + string(filename);
#endif
        if (std::filesystem::exists(overridePath))
        {
            printf("Found override for \"%s\"\n", filename);

            //Load string from file and allocate buffer for it with RFG mempool
            string fileString = File::ReadToString(overridePath);
            char* fileBuffer = (char*)dest->alloc(fileString.size() + 1, 4);
            if (!fileBuffer)
            {
                printf("Error: Failed to allocate buffer for xml override. Reverting to default xml parser.\n");
                return xml_parse_hook.CallTarget(filename, dest);
            }

            //Copy string to RFG buffer and null terminate the buffer
            memcpy(fileBuffer, fileString.data(), fileString.size());
            fileBuffer[fileString.size()] = '\0';

            //Parse xml string
            xml_element* root = functions->xml_parse_from_string(fileBuffer, dest, filename);

            //Return xml root if parse was successful
            if (root)
            {
                return root;
            }
            else
            {
                printf("Error: xml_parse_from_string() failed for xml override. Reverting to default xml parser.\n");
                return xml_parse_hook.CallTarget(filename, dest);
            }
        }
        else //No override found. Load default files from vpps
        {
            return xml_parse_hook.CallTarget(filename, dest);
        }
    }
};