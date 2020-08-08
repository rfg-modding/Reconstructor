#include "DebugGui.h"
#include "guiBase/ImportedFunctions.h"
#include "rsl2/misc/GlobalState.h"
#include "RFGR_Types/rfg/Player.h"
#include <IconFontCppHeaders/IconsFontAwesome5_c.h>

//Todo: Make version of this that works in all plugins
template<class T>
T OffsetPtr(unsigned long Offset)
{
    static RSL2_GlobalState* globalState = ext_GetGlobalState();
    if (globalState->ModuleBase == 0)
    {
        globalState->ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
    }
    return reinterpret_cast<T>(globalState->ModuleBase + Offset);
}

void DebugGui_DoFrame()
{
    static RSL2_GlobalState* globalState = ext_GetGlobalState();
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

    ImGui::End();
}

void DebugOverlay_DoFrame()
{
    static RSL2_GlobalState* globalState = ext_GetGlobalState();
    static rfg::RfgFunctions* Functions = ext_GetRfgFunctions();
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