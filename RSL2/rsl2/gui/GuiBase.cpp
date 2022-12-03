#include "GuiBase.h"
#include "rsl2/misc/GlobalState.h"
#include <imgui.h>
#include <vector>

struct Gui
{
    string Name;
    GuiUpdateFunction Update;
    bool Open;

    Gui(string name, GuiUpdateFunction update, bool open = false) : Name(name), Update(update), Open(open)
    {

    }
};

std::vector<Gui> Guis;

void DrawMainMenuBar();
void DrawDockspace();

void DrawCustomGui()
{
    static RSL2_GlobalState* state = GetGlobalState();
    if (!state->ImGuiInitialized || !state->GuiActive)
        return;

    //Draw main menu bar and dockspace
    DrawMainMenuBar();
    DrawDockspace();
#if DEBUG_BUILD
    ImGui::ShowDemoWindow();
#endif

    //Draw guis
    for (Gui& gui : Guis)
        gui.Update(&gui.Open);
}

void AddCustomGui(const std::string& name, GuiUpdateFunction update)
{
    auto find = std::find_if(Guis.begin(), Guis.end(), [name](Gui& gui) -> bool { return gui.Name == name; });
    if (find == Guis.end())
    {
        Guis.emplace_back(name, update, false);
    }
    else
    {
        printf("Tried to add the custom gui '%s' when it already exists.", name.c_str());
    }
}

void RemoveCustomGui(const std::string& name)
{
    auto find = std::find_if(Guis.begin(), Guis.end(), [name](Gui& gui) -> bool { return gui.Name == name; });
    if (find != Guis.end())
    {
        Guis.erase(find);
    }
}

void DrawMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            for (Gui& gui : Guis)
            {
                ImGui::MenuItem(gui.Name.c_str(), "", &gui.Open);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void DrawDockspace()
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