#pragma once

class IHost;
class IReconstructor;
class ILuaScripting;

extern IHost* host_;
extern IReconstructor* reconstructor;
extern ILuaScripting* luaScripting_;
extern bool ImGuiContextInitialized;