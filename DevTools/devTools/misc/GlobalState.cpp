#include "GlobalState.h"

IHost* host_ = nullptr;
IReconstructor* reconstructor = nullptr;
ILuaScripting* luaScripting_ = nullptr;
bool ImGuiContextInitialized = false;