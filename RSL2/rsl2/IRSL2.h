#pragma once

//Alias for callback function pointer types
using ImGuiCallbackFunc = void(__cdecl*)();
using OverlayCallbackFunc = void(__cdecl*)();
using PrimitiveDrawCallbackFunc = void(__cdecl*)();

//Forward declarations so we're not forcing other plugins to include things til they need them
struct ImGuiContext;
class RSL2_GlobalState;
namespace rfg
{
    class RfgFunctions;
}

//Function interface exposed by main RSL2 plugin. Used for cross-plugin interaction
class IRSL2
{
public:
    RSL2_GlobalState* (*GetGlobalState)() = nullptr;
    rfg::RfgFunctions* (*GetRfgFunctions)() = nullptr;
    ImGuiContext* (*GetImGuiContext)() = nullptr;
    void (*RegisterImGuiCallback)(ImGuiCallbackFunc callback) = nullptr;
    void (*RemoveImGuiCallback)(ImGuiCallbackFunc callback) = nullptr;
    void (*RegisterOverlayCallback)(OverlayCallbackFunc callback) = nullptr;
    void (*RemoveOverlayCallback)(OverlayCallbackFunc callback) = nullptr;
    void (*RegisterPrimitiveDrawCallback)(PrimitiveDrawCallbackFunc callback) = nullptr;
    void (*RemovePrimitiveDrawCallback)(PrimitiveDrawCallbackFunc callback) = nullptr;
};