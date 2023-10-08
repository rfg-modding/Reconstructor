#include "MiscToolsGui.h"
#include "devTools/misc/GlobalState.h"
#include "reconstructor/IReconstructor.h"
#include "reconstructor/misc/GlobalState.h"
#include "RFGR_Types/rfg/Player.h"
#include <imgui/imgui.h>

MiscToolsGui::MiscToolsGui()
{
    Open = false;
    Widget = false;
    Name = "Misc Tools GUI";
}

//TODO: Come up with a better way of doing this. Maybe have each plugin that uses ImGui register a callback with Reconstructor.dll that'll handle calling them as necessary.
void InitializeImGui()
{
	if (!ImGuiContextInitialized)
	{
		//This needs to be called on all plugins that use dear imgui to make sure they're all using one context
		ImGui::SetCurrentContext(reconstructor->GetImGuiContext());
		ImGuiContextInitialized = true;
	}
}

void MiscToolsGui::Draw()
{
	InitializeImGui();
    Reconstructor_GlobalState* state = reconstructor->GetGlobalState();

    if (!ImGui::Begin("Misc dev tools"))
    {
        ImGui::End();
        return;
    }

    if (state && state->Player && state->MainCamera)
    {
        vector cameraPos = state->MainCamera->real_pos;
        vector playerPos = state->Player->pos;
        ImGui::InputFloat3("Player pos", (float*)&playerPos);
        ImGui::InputFloat3("Camera pos", (float*)&cameraPos);
    }
    else
    {
        ImGui::Text("Player* is null. Load a map or save.");
    }

    ImGui::End();
}

void MiscToolsGui::MainMenu()
{
	InitializeImGui();

    if (ImGui::BeginMenu("Tools"))
    {
        if (ImGui::MenuItem("Dev tools"))
        {
            Open = true;
        }
        ImGui::EndMenu();
    }
}