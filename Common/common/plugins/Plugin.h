#pragma once
#include "common/Typedefs.h"
#include "common/windows/WindowsWrapper.h"
#include "common/plugins/IHost.h"
#include <filesystem>
#include <vector>

//Function exported by a plugin. Allows plugins to expose functions to each other easily
class PluginFunction
{
public:
    void* Ptr = nullptr;
    string Name;
};

//Config info about the plugin used by the host
class PluginInfo
{
public:
    std::vector<string> Dependencies;
};

//Function signatures that plugin DLLs should export if they wish to be loaded by the host DLL

//Called before init. Returns a list of other plugins it depends on to decide on load order
using PluginInfoFuncPtr = PluginInfo(__cdecl*)();

//Called when a dependency loads/unloads respectively. Necessary to fix callbacks or pointers that the dependency owns.
using PluginOnDependencyUnloadFuncPtr = void(__cdecl*)(const string& dependencyName); //Called immediately before plugin unload
using PluginOnDependencyLoadFuncPtr = void(__cdecl*)(const string& dependencyName); //Called immediately after plugin load. This is only called during reloads of the dep.

//Called when the plugin is being loaded. Function hooks and patches should go here. 
//Exported name must be RSL2_PluginInit
using PluginInitFuncPtr = bool(__cdecl*)(IHost* host, std::vector<PluginFunction>& exportedFunctions);

//Called when the plugin is being unloaded or reloaded. Should reset hooks and patches here + deallocate anything allocated by the plugin. 
//Exported name must be RSL2_PluginUnload
using PluginShutdownFuncPtr = void(__cdecl*)();


//Responsible for checking if the plugin needs to be reloaded + stores pointers to it's interface functions for the host app
class Plugin
{
public:
    Plugin(const string& path, IHost* host);
    bool Load();
    void Unload();
    bool Init();
    void Shutdown();
    void OnDependencyUnload(const string dependencyName);
    void OnDependencyLoad(const string dependencyName);

    bool NeedsReload();
    bool Reload();
    bool DependsOn(const string& targetDependency);
    string Path() const { return path_; }
    string Name() const { return name_; }

    PluginInfoFuncPtr InfoFuncPtr = nullptr;
    PluginOnDependencyUnloadFuncPtr OnDependencyUnloadFuncPtr = nullptr;
    PluginOnDependencyLoadFuncPtr OnDependencyLoadFuncPtr = nullptr;
    PluginInitFuncPtr InitFuncPtr = nullptr;
    PluginShutdownFuncPtr ShutdownFuncPtr = nullptr;

    std::vector<PluginFunction> ExportedFunctions;
    std::vector<string> Dependencies;

    //True when initialized, false when shutdown
    bool Active = false;
    bool Remove = false;

private:
    std::filesystem::file_time_type pluginLastWriteTime_;
    string path_;
    string name_;
    HINSTANCE pluginHandle_ = nullptr;
    IHost* host_ = nullptr;
};
