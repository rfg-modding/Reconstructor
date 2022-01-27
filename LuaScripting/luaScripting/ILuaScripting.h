#pragma once

//Forward declarations so we're not forcing other plugins to include things til they need them
namespace sol
{
    class state;
}

//Function interface exposed by LuaScripting.dll for cross-plugin interaction
class ILuaScripting
{
public:
    sol::state& (*GetLuaState)() = nullptr;
};