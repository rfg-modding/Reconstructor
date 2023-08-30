#pragma once
#include "common/Typedefs.h"

using GuiUpdateFunction = void(*)(bool* open);

//Dear ImGui based UI / overlay. This function creates a main menu bar and dockspace and draws any guis added by other plugins via AddCustomGui()
extern void DrawCustomGui();
extern void AddCustomGui(const std::string& name, GuiUpdateFunction update);
extern void RemoveCustomGui(const std::string& name);