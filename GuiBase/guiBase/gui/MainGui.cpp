#include "MainGui.h"
#include "DebugGui.h"
#include "rsl2/misc/GlobalState.h"
#include "RFGR_Types/rfg/Player.h"
#include "rsl2/functions/Functions.h"
#include <IconFontCppHeaders/IconsFontAwesome5_c.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <vector>

//Todo: Make list of debug guis + state like old RSL to reduce boilerplate
    //Todo: Would be nice if it could support both IGuiModules and free function or lambdas
        //Todo: Consider if this would backfire and make code a mess
    //Todo: Make accessible from other DLLs so they can add new tools or menus to the main menu bar
    //Todo: Support overlays too
static bool MiscTweaksGui_Open = true;

void DrawMainMenuBar(IRSL2* rsl2);
void DrawDockspace(IRSL2* rsl2);

void MainGui_DoFrame(IRSL2* rsl2)
{
    RSL2_GlobalState* globalState = rsl2->GetGlobalState();
    if (!globalState)
        return;

    DrawMainMenuBar(rsl2);
    DrawDockspace(rsl2);

    if (MiscTweaksGui_Open)
        DebugGui_DoFrame(rsl2, &MiscTweaksGui_Open);
}

void DrawMainMenuBar(IRSL2* rsl2)
{
    RSL2_GlobalState* globalState = rsl2->GetGlobalState();
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("Misc tweaks gui", "", &MiscTweaksGui_Open);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void DrawDockspace(IRSL2* rsl2)
{
    RSL2_GlobalState* globalState = rsl2->GetGlobalState();

    //Dockspace flags
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    //Parent window flags
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImVec2 dockspaceSize = viewport->WorkSize;
    ImGui::SetNextWindowSize(dockspaceSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace parent window", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    //DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspaceId = ImGui::GetID("Editor dockspace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    ImGui::End();
}