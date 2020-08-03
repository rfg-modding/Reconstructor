#pragma once
#include "common/Typedefs.h"
#include "rsl2/misc/GlobalState.h"
#include "rsl2/functions/Functions.h"
#include "imgui.h"

//Todo: Make this less of a fucking mess. Maybe just use virtual function interfaces and 1 exported function that returns that
//External functions used by this plugin
using F_ext_GetGlobalState = RSL2_GlobalState*(__cdecl*)();
extern F_ext_GetGlobalState ext_GetGlobalState;

using F_ext_GetRfgFunctions = rfg::RfgFunctions*(__cdecl*)();
extern F_ext_GetRfgFunctions ext_GetRfgFunctions;

using F_ext_GetImGuiContext = ImGuiContext*(__cdecl*)();
extern F_ext_GetImGuiContext ext_GetImGuiContext;

//Imgui callback signature
using ImGuiCallbackFunc = void(__cdecl*)();
//Signature and pointer to RegisterImGuiCallback
using F_ext_RegisterImGuiCallback = void(__cdecl*)(ImGuiCallbackFunc callback);
extern F_ext_RegisterImGuiCallback ext_RegisterImGuiCallback;
//Signature and pointer to RemoveImGuiCallback
using F_ext_RemoveImGuiCallback = void(__cdecl*)(ImGuiCallbackFunc callback);
extern F_ext_RemoveImGuiCallback ext_RemoveImGuiCallback;

//Overlay callback signature
using OverlayCallbackFunc = void(__cdecl*)();
//Signature and pointer to RegisterOverlayCallback
using F_ext_RegisterOverlayCallback = void(__cdecl*)(OverlayCallbackFunc callback);
extern F_ext_RegisterOverlayCallback ext_RegisterOverlayCallback;
//Signature and pointer to RemoveOverlayCallback
using F_ext_RemoveOverlayCallback = void(__cdecl*)(OverlayCallbackFunc callback);
extern F_ext_RemoveOverlayCallback ext_RemoveOverlayCallback;

//Primitive draw callback signature
using PrimitiveDrawCallbackFunc = void(__cdecl*)();
//Signature and pointer to RegisterPrimitiveDrawCallback
using F_ext_RegisterPrimitiveDrawCallback = void(__cdecl*)(PrimitiveDrawCallbackFunc callback);
extern F_ext_RegisterPrimitiveDrawCallback ext_RegisterPrimitiveDrawCallback;
//Signature and pointer to RemovePrimitiveDrawCallback
using F_ext_RemovePrimitiveDrawCallback = void(__cdecl*)(PrimitiveDrawCallbackFunc callback);
extern F_ext_RemovePrimitiveDrawCallback ext_RemovePrimitiveDrawCallback;