#pragma once
#include <string>
#include "gui/OverlayGuis.h"

//Alias for callback function pointer types
using ImGuiCallbackFunc = void(__cdecl*)();
using OverlayCallbackFunc = void(__cdecl*)();
using PrimitiveDrawCallbackFunc = void(__cdecl*)();
using GuiUpdateFunction = void(*)(bool* open);

//Forward declarations so we're not forcing other plugins to include things til they need them
struct ImGuiContext;
class Reconstructor_GlobalState;
namespace rfg
{
    class RfgFunctions;
}

//Function interface exposed by main Reconstructor plugin. Used for cross-plugin interaction
class IReconstructor
{
public:
    Reconstructor_GlobalState* (*GetGlobalState)() = nullptr;
    rfg::RfgFunctions* (*GetRfgFunctions)() = nullptr;
    ImGuiContext* (*GetImGuiContext)() = nullptr;
    void (*RegisterPrimitiveDrawCallback)(PrimitiveDrawCallbackFunc callback) = nullptr;
    void (*RemovePrimitiveDrawCallback)(PrimitiveDrawCallbackFunc callback) = nullptr;
    void (*RegisterGui)(GuiBase* gui) = nullptr;
    void (*RemoveGui)(GuiBase* gui) = nullptr;
};