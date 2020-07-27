#pragma once
#include "common/Typedefs.h"
#include "common/windows/WindowsWrapper.h"
#include <filesystem>

//Function signatures that plugin DLLs should export if they wish to be loaded by the host DLL

//Called when the plugin is being loaded. Function hooks and patches should go here. 
//Exported name must be RSL2_PluginInit
using PluginInitFuncPtr = bool(__cdecl*)();
//Called when the plugin is being unloaded or reloaded. Should reset hooks and patches here + deallocate anything allocated by the plugin. 
//Exported name must be RSL2_PluginUnload
using PluginUnloadFuncPtr = void(__cdecl*)();


//Responsible for checking if the plugin needs to be reloaded + stores pointers to it's interface functions for the host app
class Plugin
{
public:
    Plugin(const string& path);
    bool Load();
    void Unload();
    bool NeedsReload();
    bool Reload();
    string Path() const { return path_; }

    PluginInitFuncPtr InitFuncPtr = nullptr;
    PluginUnloadFuncPtr UnloadFuncPtr = nullptr;

private:
    std::filesystem::file_time_type pluginLastWriteTime_;
    string path_;
    HINSTANCE pluginHandle_ = nullptr;
};
