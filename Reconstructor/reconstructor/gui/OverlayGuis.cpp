#include "OverlayGuis.h"
#include "reconstructor/misc/GlobalState.h"
#include <imgui.h>
#include <vector>

OverlayGuis& OverlayGuis::Instance()
{
    static OverlayGuis instance;
    return instance;
}

void OverlayGuis::RegisterGui(GuiBase* newGui)
{
    auto find = std::find_if(Guis.begin(), Guis.end(), [newGui](GuiBase* gui) -> bool { return gui == newGui; });
    if (find == Guis.end())
    {
        Guis.emplace_back(newGui);
    }
    else
    {
        printf("Tried to add the custom gui '%s' when it already exists.", newGui->Name.c_str());
    }
}

void OverlayGuis::RemoveGui(GuiBase* guiToRemove)
{
    auto find = std::find_if(Guis.begin(), Guis.end(), [guiToRemove](GuiBase* gui) -> bool { return gui == guiToRemove; });
    if (find != Guis.end())
    {
        Guis.erase(find);
    }
}

void OverlayGuis::Draw()
{
    static Reconstructor_GlobalState* state = GetGlobalState();

    //Draw main menu bar and dockspace
    if (state->GuiVisible)
    {
        DrawMainMenuBar();
        DrawDockspace();
    }

    //Draw guis
    if (state->GuiVisible)
    {
        for (GuiBase* gui : Guis)
        {
            if (gui->Open && !gui->Widget)
            {
                gui->Draw();
            }
        }
    }

    //Draw widgets
    if (state->WidgetsVisible)
    {
        for (GuiBase* gui : Guis)
        {
            if (gui->Open && gui->Widget)
            {
                gui->Draw();
            }
        }
    }

#if DEBUG_BUILD
    if (state->GuiVisible)
    {
        ImGui::ShowDemoWindow();
    }
#endif
}

void OverlayGuis::DrawDockspace()
{
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

void OverlayGuis::DrawMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        for (GuiBase* gui : Guis)
        {
            gui->MainMenu();
        }

        ImGui::EndMainMenuBar();
    }
}