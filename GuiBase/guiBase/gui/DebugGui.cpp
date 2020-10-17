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