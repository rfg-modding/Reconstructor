#include "DebugGui.h"
#include "rsl2/misc/GlobalState.h"
#include "RFGR_Types/rfg/Player.h"
#include "rsl2/functions/Functions.h"
#include "KnownCues.h"
#include <IconFontCppHeaders/IconsFontAwesome5_c.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <vector>

//int __cdecl audiolib_cue_play(int cue_id, audiolib_params *passed_params, audiolib_result *error) //0x0009F100
//audiolib_result __cdecl audiolib_cue_get_id(const char *cue_name, int *caller_cue_id) //0x0008DC20

int lastSoundPlayed = 0;
void PlaySound(const char* name, rfg::RfgFunctions* functions)
{
    int cue_id = 0;
    audiolib_result result = functions->audiolib_cue_get_id(name, &cue_id);
    if (result != AUDIOLIB_ERR_NONE)
    {
        printf("Error encountered in PlaySound() when trying to find cue \"%s\". Error code: %d\n", name, (int)result);
        return;
    }
    int playId = (audiolib_result)functions->audiolib_cue_play(cue_id, nullptr, &result);
    if (result != AUDIOLIB_ERR_NONE)
    {
        printf("Error encountered in PlaySound() when trying to play cue \"%s\". Error code: %d. Return value: %d\n", name, (int)result, (int)playId);
        return;
    }
    lastSoundPlayed = playId;
}

void DebugGui_DoFrame(IRSL2* rsl2)
{
    RSL2_GlobalState* globalState = rsl2->GetGlobalState();
    if (!globalState || !globalState->Player || !globalState->MainCamera || !globalState->World)
        return;

    if (!ImGui::Begin("Debug gui"))
    {
        ImGui::End();
        return;
    }

    ImGui::PushFont(globalState->FontLarge);
    ImGui::Text(ICON_FA_BUG " Debug");
    ImGui::PopFont();
    ImGui::Separator();

    ImGui::PushFont(globalState->FontLarge);
    ImGui::Text(ICON_FA_AUDIO_DESCRIPTION " Audio");
    ImGui::PopFont();
    ImGui::Separator();

    rfg::RfgFunctions* Functions = rsl2->GetRfgFunctions();

    static string input;
    ImGui::InputText("Custom sound name", &input);
    ImGui::SameLine();
    if (ImGui::Button("Play custom sound"))
    {
        PlaySound(input.c_str(), Functions);
    }
    if (ImGui::Button("Stop last sound"))
    {
        Functions->audiolib_play_fade_out(lastSoundPlayed, 100);
    }

    ImGui::BeginChild("##Sound presets list", ImVec2(0, 0), true);
    if (ImGui::CollapsingHeader("Known sounds"))
    {
        for (const char* presetName : SoundPresets)
        {
            if (ImGui::Button(presetName))
            {
                PlaySound(presetName, Functions);
            }
        }
    }
    ImGui::EndChild();
    

    ImGui::Separator();
    ImGui::PushFont(globalState->FontLarge);
    ImGui::Text(ICON_FA_PLANE_ARRIVAL " Air bombs test");
    ImGui::PopFont();
    ImGui::Separator();

    //template<class T>
    //T OffsetPtr(unsigned long Offset)
    //{
    //    static RSL2_GlobalState* globalState = GetGlobalState();
    //    if (globalState->ModuleBase == 0)
    //    {
    //        globalState->ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
    //    }
    //    return reinterpret_cast<T>(globalState->ModuleBase + Offset);
    //}

    static air_bomb_info customAirBomb;
    static bool firstRun = true;
    static u32 targetObjectHandle = 0xFFFFFFFF;
    static int lastAirBomb = -1;
    if (firstRun)
    {
        //Todo: Use OffsetPtr<T> here
        //Todo: Come up with better way of initing these things post RSL2.dll init so their values aren't null like they might be at game start
        globalState->NumAirBombInfos = reinterpret_cast<u32*>(globalState->ModuleBase + 0x01E28610);
        auto airBombInfosPtr = reinterpret_cast<air_bomb_info**>(globalState->ModuleBase + 0x01E2860C);
        globalState->AirBombInfos.Init(*airBombInfosPtr, *globalState->NumAirBombInfos, *globalState->NumAirBombInfos, "AirBombInfos");

        customAirBomb = globalState->AirBombInfos[0];
        firstRun = false;
    }

    ImGui::PushItemWidth(250.0f);
    ImGui::InputScalar("Target handle", ImGuiDataType_U32, &targetObjectHandle);
    ImGui::SameLine();
    if (ImGui::Button("Player aim"))
    {
        targetObjectHandle = globalState->Player->aim_target;
    }
    ImGui::SameLine();
    if (ImGui::Button("Player handle"))
    {
        targetObjectHandle = globalState->Player->handle;
    }
    if (ImGui::Button("Start air bomb"))
    {
        lastAirBomb = Functions->air_bomb_start(&customAirBomb, targetObjectHandle, globalState->Player->handle, false);
    }
    if (ImGui::Button("Stop last air bomb"))
    {
        if (lastAirBomb != -1)
        {
            Functions->air_bomb_stop((air_bomb_handle)lastAirBomb);
        }
    }
    if (ImGui::Button("Stop all air bombs"))
    {
        Functions->air_bomb_stop_all();
    }

    if (ImGui::TreeNode("Spawn settings"))
    {
        ImGui::Separator();
        ImGui::PushFont(globalState->FontLarge);
        ImGui::Text("Air bomb info:");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::PushItemWidth(250.0f);
        ImGui::Text("name: %s", customAirBomb.name);
        //Todo: Add weapon_info combo here. Presets don't use any interesting projectiles
        //Todo: Add projectile spawner
        ImGui::InputInt("num_projectiles", &customAirBomb.num_projectiles);
        ImGui::InputFloat("min_spread", &customAirBomb.min_spread);
        ImGui::InputFloat("max_spread", &customAirBomb.max_spread);
        ImGui::InputFloat("min_projectile_delay", &customAirBomb.min_projectile_delay);
        ImGui::InputFloat("max_projectile_delay", &customAirBomb.max_projectile_delay);
        ImGui::InputFloat("height", &customAirBomb.height);
        ImGui::InputFloat("radius", &customAirBomb.radius);
        ImGui::InputFloat("veh_radius", &customAirBomb.veh_radius);
        ImGui::InputFloat("rate_min", &customAirBomb.rate_min);
        ImGui::InputFloat("rate_max", &customAirBomb.rate_max);
        ImGui::InputFloat("projectile_travel_time", &customAirBomb.projectile_travel_time);
        ImGui::InputFloat("angle_min", &customAirBomb.angle_min);
        ImGui::InputFloat("angle_max", &customAirBomb.angle_max);
        ImGui::InputFloat("min_player_fvec_dot_prod", &customAirBomb.min_player_fvec_dot_prod);

        ImGui::TreePop();
    }

    if (globalState->AirBombInfos.Initialized() && ImGui::TreeNode("Preset air bombs"))
    {
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 260.0f);
        for (int i = 0; i < globalState->AirBombInfos.Size(); i++)
        {
            ImGui::BulletText("%s", globalState->AirBombInfos[i].name);
            ImGui::SameLine();
            ImGui::NextColumn();
            ImGui::SetColumnWidth(1, 180.0f);

            if (ImGui::Button((string("Copy values##") + std::to_string(i)).c_str()))
                customAirBomb = globalState->AirBombInfos[i];

            ImGui::NextColumn();
        }
        ImGui::TreePop();
    }
    ImGui::Columns(1);

    ImGui::End();
}

void DebugOverlay_DoFrame(IRSL2* rsl2)
{
    RSL2_GlobalState* globalState = rsl2->GetGlobalState();
    static bool p_open = true;

    // FIXME-VIEWPORT: Select a default viewport
    const float DISTANCE = 10.0f;
    static int corner = 0;
    ImGuiIO& io = ImGui::GetIO();
    if (corner != -1)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_area_pos = viewport->GetWorkPos();   // Instead of using viewport->Pos we use GetWorkPos() to avoid menu bars, if any!
        ImVec2 work_area_size = viewport->GetWorkSize();
        ImVec2 window_pos = ImVec2((corner & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE) :
            (work_area_pos.x + DISTANCE), (corner & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE) : (work_area_pos.y + DISTANCE));
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
    }
    ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != -1)
        window_flags |= ImGuiWindowFlags_NoMove;

    if (ImGui::Begin("Example: Simple overlay", &p_open, window_flags))
    {
        ImGui::Text("FPS: %.2f", io.Framerate);
        ImGui::Separator();
        if (globalState->Player)
            ImGui::Text("Player Position: (%.2f,%.2f, %.2f)", globalState->Player->pos.x, globalState->Player->pos.y, globalState->Player->pos.z);
        else
            ImGui::Text("Player Position: <invalid>");

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (p_open && ImGui::MenuItem("Close")) p_open = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}