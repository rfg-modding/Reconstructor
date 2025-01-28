#include "Patches.h"
#include <asmjit/x86.h>
#include "RFGR_Types/rfg/Animation.h"
#include "common/patching/Config.h"
#include "common/patching/Offset.h"
#include "reconstructor/misc/GlobalState.h"
#include <cassert>

//Patch a push instruction to `push pushValue`. Currently used to adjust size arguments of string_pool::init() since mods can overload the vanilla string pool sizes
void PatchPushInstruction(u32 relativeAddress, u32 pushValue);
void PatchStringPoolSizeInit(u32 patchAddressA, u32 patchAddressB, u32 size);
void PatchMovValInstruction(u32 relativeAddress, asmjit::x86::Gpd reg, u32 value);
void PatchLeaInstruction(u32 relativeAddress, asmjit::x86::Gpd reg, u32 leaAddress);
void PatchOffsetLeaInstruction(u32 relativeAddress, u32 leaAddress);
void PatchCmpValInstruction(u32 relativeAddress, asmjit::x86::Gpd reg, u32 value);

void PatchLeaInstruction2(u32 relativeAddress, asmjit::x86::Gpd reg, u32 leaAddress, u32 shift)
{
    Reconstructor_GlobalState* globalState = GetGlobalState();
    uintptr_t moduleBase = globalState->ModuleBase;
    DWORD absoluteAddress = moduleBase + relativeAddress;
    DWORD absoluteLeaAddress = moduleBase + leaAddress;

    //Holds generated code
    asmjit::CodeHolder code;
    code.init(asmjit::Environment::host(), asmjit::CpuInfo::host().features());

    //Generate code
    asmjit::x86::Assembler assembler(&code);
    assembler.lea(reg, asmjit::x86::ptr(leaAddress, asmjit::x86::esi, shift, relativeAddress));

    //Apply generated code to games code
    globalState->Patcher.Replace(absoluteAddress, code);
}

void ApplyPatches()
{
    Reconstructor_GlobalState* globalState = GetGlobalState();
    uintptr_t moduleBase = globalState->ModuleBase;

    /* Patch mempool max sizes */

    //Common_page_pool. Vanilla is 13369344
    PatchPushInstruction(0x001D865F, 26738688);
    //Permanent_mempool. Vanilla is 15728640
    PatchPushInstruction(0x001D8E35, 31457280);
    //MP_effect_preload_mempool. Vanilla size is 5767168. Only cpu_mem_size is patched since gpu_mem_size = 0
    PatchPushInstruction(0x0054F359, 11534336);
    //MP_item_preload_mempool. Vanilla size is 80735232
    PatchPushInstruction(0x0054F3A2, 161470464); //TODO: May need to increase size of pool used by Stream_mp_effect_preload_allocator (source allocator)
    //Item_preload_pool. Vanilla is 134217728
    PatchPushInstruction(0x001D8F2B, 268435456);

    //Patch Anim_rig array. Default size is 290 entries. Need more for modding
    //PatchPushInstruction(0x0040C032, 65536); //Increase temporary mempool in game_rig_system_init(). Default size is 32768.
    /*WIP anim_rig array size increase. Default size is 290. Can cause problems with mod. Code is commented out currently since it still has crash bugs*/
    //{
    //    const u32 numAnimRigs = 500;
    //    const uintptr_t originalArray = moduleBase + 0x034BC6F8;
    //    globalState->OriginalAnimRigArray = (anim_rig*)(moduleBase + 0x034BC6F8);

    //    //The array is static so first we need to make our own array. Then we patch all spots in the game code that use the array or its original size
    //    anim_rig* customAnimRigs = new anim_rig[numAnimRigs];
    //    memset(customAnimRigs, 0, numAnimRigs * sizeof(anim_rig));
    //    const u8* customAnimRigsEnd = ((u8*)customAnimRigs) + (numAnimRigs * sizeof(anim_rig));
    //    const u8* customAnimRigsLastEntry = customAnimRigsEnd - sizeof(anim_rig);

    //    //Patch all uses of original array to point to the new one
    //    //anim_rig_system_init() - Not patched since it's just clearing the original array. We handle that ourselves
    //    //0x0088AA91 - mov instruction in anim_rig_system_shutdown() - Not patched since the game doesn't call this code
    //    PatchMovValInstruction(0x0088AAC9, asmjit::x86::esi, (u32)customAnimRigs); //anim_rig_find_index() - loops through array
    //    //0x008908A7 - push in animlib_system_init() - Not patched since it's just doing a memset on the original array.
    //    PatchMovValInstruction(0x008908C1, asmjit::x86::esi, (u32)customAnimRigs); //animlib_system_close() - loops through array and zeroes each element
    //    PatchLeaInstruction(0x00890944, asmjit::x86::eax, (u32)customAnimRigs); //animlib_get_loaded_rig()
    //    PatchMovValInstruction(0x00895875, asmjit::x86::eax, (u32)customAnimRigs); //anim_rig_new()
    //    PatchOffsetLeaInstruction(0x008958B9, (u32)customAnimRigs); //anim_rig_new()
    //    PatchCmpValInstruction(0x00895889, asmjit::x86::eax, (u32)customAnimRigsEnd); //anim_rig_new() - Patch sentinel value check that sees iterated past the end of the rigs array
    //    PatchCmpValInstruction(0x0088AAE8, asmjit::x86::esi, (u32)customAnimRigsLastEntry); //anim_rig_find_index() - Patch sentinel value. Similar to the last one but checks for the last element instead.
    //    PatchCmpValInstruction(0x0040C064, asmjit::x86::ebp, numAnimRigs); //game_rig_system_init(). Hardcoded 290

    //    globalState->CustomAnimRigArray = customAnimRigs;
    //}

    //Patch inventory items array to be larger. It's fixed size so we have to allocate a new one and patch the code to use that array + the new max size instead of the old one
    {
        const u32 maxInventoryItems = 127;
        assert(maxInventoryItems <= 127); //Must not exceed this since the vanilla instruction is comparing against a signed byte. Any bigger and we need more space for the instructions. Don't want to screw with relocating other instructions at the moment.
        const uintptr_t originalArray = moduleBase + 0x02C6D550;
        globalState->OriginalInventoryItemArray = (inv_item_info*)originalArray;

        //The array is static so first we need to make our own array. Then we patch all spots in the game code that use the array or its original size  
        inv_item_info* customInvItems = new inv_item_info[maxInventoryItems];
        memset(customInvItems, 0, maxInventoryItems * sizeof(inv_item_info));
        const u8* customInvItemsEnd = ((u8*)customInvItems) + (maxInventoryItems * sizeof(inv_item_info));
        const u8* customInvItemsLastEntry = customInvItemsEnd - sizeof(inv_item_info);
        globalState->CustomInventoryItemArray = customInvItems;

        //Patch all uses of the original array to point to the new one
        //inv_item_parse()
        //TODO: Give this function a better name
        PatchLeaInstruction2(0x0061203A, asmjit::x86::esi, (u32)customInvItems, 3); //TODO: Check disassembly for assembly pattern and put it in a comment here so I remember what sort of patch this is (use online disassembler to strip name). Makes future patches easier
        PatchCmpValInstruction(0x00612013, asmjit::x86::eax, maxInventoryItems);
        //inv_item_get_info()
        PatchMovValInstruction(0x00612251, asmjit::x86::edi, (u32)customInvItems);
        //inv_item_info_from_handle()
        PatchMovValInstruction(0x00612381, asmjit::x86::ecx, (u32)customInvItems + offsetof(inv_item_info, name_checksum));
        PatchLeaInstruction2(0x00612399, asmjit::x86::eax, (u32)customInvItems, 3);
        //player::load_inventory()
        PatchMovValInstruction(0x006C2FFB, asmjit::x86::edi, (u32)customInvItems);
        PatchMovValInstruction(0x006C3050, asmjit::x86::edi, (u32)customInvItems);

        auto debug = 0;
    }

    /* Patch string pool max sizes */

    //Upgrades_string_pool. Vanilla is 2048
    PatchStringPoolSizeInit(0x00390D3B, 0x00390D6E, 4096);
    //Area_defense_string_pool. Vanilla is 1024
    PatchStringPoolSizeInit(0x00396249, 0x0039627C, 2048);
    //Riding_shotgun_string_pool. Vanilla is 512
    PatchStringPoolSizeInit(0x00399E3D, 0x00399E72, 2048);

    //Delivery_string_pool. Vanilla is 512
    PatchStringPoolSizeInit(0x0039A61F, 0x0039A650, 1024);

    //Raid_string_pool. Vanilla is 1024
    PatchStringPoolSizeInit(0x0039B10A, 0x0039B13D, 2048);

    //Wrecking_crew_string_pool. Vanilla is 3072
    PatchStringPoolSizeInit(0x0039EDD5, 0x0039EDF3, 6144);

    //Cont_attack_string_pool. Vanilla is 896
    PatchStringPoolSizeInit(0x003A0C6B, 0x003A0C9E, 1792);

    //Squad_name_string_pool. Vanilla is 2048. The game has two functions which initialize it. One makes it 768 bytes so I just picked the bigger one.
    PatchStringPoolSizeInit(0x003A2D5D, 0x003A2D8E, 4096);

    //Managed_layer_name_string_pool. Vanilla is 6144
    PatchStringPoolSizeInit(0x003A573B, 0x003A576B, 12288);

    //Activity_mission_menu_string_pool. Vanilla is 6656
    //PatchStringPoolSizeInit(0x, 0x, 2048); //Not patched yet. Has static array so it requires a different patching technique

    //Activity_data_name_string_pool. Vanilla is 2560
    PatchStringPoolSizeInit(0x003B375B, 0x003B378E, 2560);

    //Mission_name_string_pool. Vanilla is 1450
    PatchStringPoolSizeInit(0x003B514B, 0x003B517E, 2900);

    //Stats_string_pool. Vanilla is 30720
    PatchStringPoolSizeInit(0x003D3A08, 0x003D3A3B, 61440);

    //Handbook_string_pool. Vanilla is 5000
    PatchStringPoolSizeInit(0x0047715B, 0x0047718E, 10000);

    //UI_string_pool. Vanilla is 10000
    PatchStringPoolSizeInit(0x004B703C, 0x004B7071, 20000);

    //Tool_tips_string_pool. Vanilla is 6144
    PatchStringPoolSizeInit(0x004EB765, 0x004EB796, 12288);
    PatchStringPoolSizeInit(0x004EBE4E, 0x004EBE7F, 12288);

    //Credits_string_pool. Vanilla is 131072
    PatchStringPoolSizeInit(0x00509663, 0x005096A0, 262144);

    //Multi_xp_string_pool. Vanilla is 6144
    PatchStringPoolSizeInit(0x005A354D, 0x005A3559, 12288);

    //Multi_game_character_string_pool. Vanilla is 1400
    PatchStringPoolSizeInit(0x005DCFFC, 0x005DD00B, 2800);
    PatchStringPoolSizeInit(0x005FCE57, 0x005FCE66, 2800);

    //Backpack_string_pool. Vanilla is 1024
    PatchStringPoolSizeInit(0x005DF0A2, 0x005DF0C4, 2048);

    //Human_hap_string_pool. Vanilla is 2048
    //PatchStringPoolSizeInit(0x, 0x, 4096); //Not patched yet. Has static array so it requires a different patching technique

    //Asd_string_pool. Vanilla is 512
    PatchStringPoolSizeInit(0x006609AE, 0x006609E1, 1024);

    //Gameplay_properties_name_string_pool. Vanilla is 22528
    PatchStringPoolSizeInit(0x0067ABDD, 0x0067ABF8, 45056);

    //Convoy_string_pool. Vanilla is 512
    PatchStringPoolSizeInit(0x0068E550, 0x0068E581, 1024);

    //Courier_string_pool. Vanilla is 512
    PatchStringPoolSizeInit(0x0068EEEA, 0x0068EF1B, 1024);

    //Action_node_string_pool. Vanilla is 3072
    PatchStringPoolSizeInit(0x006A4294, 0x006A42C7, 6144);

    //Demolitions_master_string_pool. Vanilla is 2048
    PatchStringPoolSizeInit(0x006A6987, 0x006A69B7, 4096);

    //House_arrest_string_pool. Vanilla is 512
    PatchStringPoolSizeInit(0x006A7067, 0x006A7097, 1024);

    //Roadblock_string_pool. Vanilla is 512
    PatchStringPoolSizeInit(0x006AB0AB, 0x006AB0C4, 1024);

    //Human_info_string_pool. Vanilla is 6200
    PatchStringPoolSizeInit(0x006BCCC4, 0x006BCCF1, 12400);

    //Ambient_spawn_string_pool. Vanilla is 1024
    PatchStringPoolSizeInit(0x007548AF, 0x007548E0, 2048);

    //Spawn_res_data_name_string_pool. Vanilla is 512
    PatchStringPoolSizeInit(0x0075C83B, 0x0075C86E, 1024);

    //Spawn_group_string_pool. Vanilla is 3584
    PatchStringPoolSizeInit(0x0075F8AB, 0x0075F8DE, 7168);

    //Vehicle_family_name_pool. Vanilla is 512
    PatchStringPoolSizeInit(0x00768B0B, 0x00768B3E, 1024);

    //Vehicle_interaction_string_pool. Vanilla is 8192
    PatchStringPoolSizeInit(0x00782403, 0x00782429, 16384);

    //Vehicle_info_string_pool. Vanilla is 8192
    PatchStringPoolSizeInit(0x007ACE81, 0x007ACEAE, 16384);
    PatchStringPoolSizeInit(0x007AFE92, 0x007AFEC2, 16384);

    //The patch was disabled by the request of Camo. He originally requested the change and now found a workaround.
    //Disable video player log spam. Appears to be caused by missing intro videos. Useful to disable them while modding for quicker startup. The error isn't useful.
    //video_player_update_render_all - call to keen::debug::nativePrintArgs("...", 1570)
    //{
    //    DWORD startAddress = moduleBase + 0x007D84B1;
    //    size_t sizeBytes = 23;
    //    globalState->Patcher.Backup("DisableAnnoyingVideoPlayerLogSpam", startAddress, sizeBytes, true);
    //}

    //Patch MP match countdown time. Vanilla is 10000ms
    {
        const int matchCountdownMS = 5000;
        const DWORD movInstructionBaseAddress = globalState->ModuleBase + 0x005AAD87; //We just need to alter the 32bit integer parameter of the mov instruction that's here.
        const DWORD timeParamAddress = movInstructionBaseAddress + 6;
        globalState->Patcher.ReplaceAndBackup("MatchCountdownTime", timeParamAddress, std::span<u8>((u8*)&matchCountdownMS, sizeof(matchCountdownMS)));
    }
}

void PatchPushInstruction(u32 relativeAddress, u32 pushValue)
{
    Reconstructor_GlobalState* globalState = GetGlobalState();
    uintptr_t moduleBase = globalState->ModuleBase;
    DWORD absoluteAddress = moduleBase + relativeAddress;

    //Holds generated code
    asmjit::CodeHolder code;
    code.init(asmjit::Environment::host(), asmjit::CpuInfo::host().features());

    //Generate code
    asmjit::x86::Assembler assembler(&code);
    assembler.push(pushValue);

    //Apply generated code to games code
    assert(code.codeSize() == 5);
    globalState->Patcher.Replace(absoluteAddress, code);
}

void PatchMovValInstruction(u32 relativeAddress, asmjit::x86::Gpd reg, u32 value)
{
    Reconstructor_GlobalState* globalState = GetGlobalState();
    uintptr_t moduleBase = globalState->ModuleBase;
    DWORD absoluteAddress = moduleBase + relativeAddress;

    //Holds generated code
    asmjit::CodeHolder code;
    code.init(asmjit::Environment::host(), asmjit::CpuInfo::host().features());

    //Generate code
    asmjit::x86::Assembler assembler(&code);
    assembler.mov(reg, value);

    //Apply generated code to games code
    globalState->Patcher.Replace(absoluteAddress, code);
}

void PatchLeaInstruction(u32 relativeAddress, asmjit::x86::Gpd reg, u32 leaAddress)
{
    Reconstructor_GlobalState* globalState = GetGlobalState();
    uintptr_t moduleBase = globalState->ModuleBase;
    DWORD absoluteAddress = moduleBase + relativeAddress;
    DWORD absoluteLeaAddress = moduleBase + leaAddress;

    //Holds generated code
    asmjit::CodeHolder code;
    code.init(asmjit::Environment::host(), asmjit::CpuInfo::host().features());

    //Generate code
    asmjit::x86::Assembler assembler(&code);
    assembler.lea(reg, asmjit::x86::ptr(leaAddress));

    //Apply generated code to games code
    globalState->Patcher.Replace(absoluteAddress, code);
}

//TODO: Implement this with AsmJIT. I couldn't figure it out and didn't want to spend ages on it so I did it by hand
//      Would be better to have a version that let you set all the arguments instead of just the address
void PatchOffsetLeaInstruction(u32 relativeAddress, u32 leaAddress)
{
    Reconstructor_GlobalState* globalState = GetGlobalState();
    uintptr_t moduleBase = globalState->ModuleBase;
    DWORD absoluteAddress = moduleBase + relativeAddress;

    u8 bytes[] = { Opcodes::LEA, 0x2C, 0xED, 0x0, 0x0, 0x0, 0x0 };
    u32* valuePtr = (u32*)&bytes[3]; //Set upper bytes of instruction to value
    *valuePtr = leaAddress;
    globalState->Patcher.Replace(absoluteAddress, bytes);
}

void PatchCmpValInstruction(u32 relativeAddress, asmjit::x86::Gpd reg, u32 value)
{
    Reconstructor_GlobalState* globalState = GetGlobalState();
    uintptr_t moduleBase = globalState->ModuleBase;
    DWORD absoluteAddress = moduleBase + relativeAddress;

    //Holds generated code
    asmjit::CodeHolder code;
    code.init(asmjit::Environment::host(), asmjit::CpuInfo::host().features());

    //Generate code
    asmjit::x86::Assembler assembler(&code);
    assembler.cmp(reg, value);

    //Apply generated code to games code
    globalState->Patcher.Replace(absoluteAddress, code);
}

void PatchStringPoolSizeInit(u32 patchAddressA, u32 patchAddressB, u32 size)
{
    PatchPushInstruction(patchAddressA, size);
    PatchPushInstruction(patchAddressB, size);
}