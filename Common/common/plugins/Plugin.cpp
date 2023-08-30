#include "Plugin.h"
#include "common/filesystem/Path.h"

Plugin::Plugin(const string& path, IHost* host)
{
    path_ = path;
    host_ = host;
    name_ = Path::GetFileNameNoExtension(path_);
}

bool Plugin::Load()
{
    //Todo: Change to load copy
    //Create copy of DLL so visual studio can write to the original copy on recompile
    string copyPath = Path::GetParentDirectory(path_) + Path::GetFileNameNoExtension(path_) + ".copy.dll";
    CopyFileA(path_.c_str(), copyPath.c_str(), FALSE);

    //Load copy dll + save write time of original dll for reload checking
    pluginHandle_ = LoadLibrary(copyPath.c_str());
    pluginLastWriteTime_ = std::filesystem::last_write_time(path_);

    //Ensure plugin dll loaded properly
    if (!pluginHandle_)
    {
        //Todo: Use a real logger
        printf("Error! Failed to load plugin at path \"%s\" Last error: %s\n", copyPath.c_str(), GetLastWin32ErrorAsString().c_str());
        FreeLibrary(pluginHandle_);
        return false;
    }

    //Get the plugin interface functions
    InfoFuncPtr = reinterpret_cast<PluginInfoFuncPtr>(GetProcAddress(pluginHandle_, "Reconstructor_PluginInfo"));
    OnDependencyUnloadFuncPtr = reinterpret_cast<PluginOnDependencyUnloadFuncPtr>(GetProcAddress(pluginHandle_, "Reconstructor_OnDependencyUnload"));
    OnDependencyLoadFuncPtr = reinterpret_cast<PluginOnDependencyLoadFuncPtr>(GetProcAddress(pluginHandle_, "Reconstructor_OnDependencyLoad"));
    InitFuncPtr = reinterpret_cast<PluginInitFuncPtr>(GetProcAddress(pluginHandle_, "Reconstructor_PluginInit"));
    ShutdownFuncPtr = reinterpret_cast<PluginShutdownFuncPtr>(GetProcAddress(pluginHandle_, "Reconstructor_PluginShutdown"));

    //Ensure the plugin dll has all the interface functions
    if (!InfoFuncPtr || !InitFuncPtr || !ShutdownFuncPtr)
    {
        printf("Error! Failed to load plugin at path \"%s\". Plugin dll is not exporting the required interface functions. See \"Common\common\plugins\Plugin.h\" for more info.\n", copyPath.c_str());
        FreeLibrary(pluginHandle_);
        return false;
    }

    //Get plugin info
    auto info = InfoFuncPtr();
    Dependencies = info.Dependencies;

    //Plugin sucessfully loaded
    return true;
}

void Plugin::Unload()
{
    //Call unload interface function so plugin can detach hooks, free allocations, etc
    ExportedFunctions.clear();
    FreeLibrary(pluginHandle_);
}

bool Plugin::Init()
{
    string copyPath = Path::GetParentDirectory(path_) + Path::GetFileNameNoExtension(path_) + ".copy.dll";

    //Call the init function and make sure it returns success (true)
    if (!InitFuncPtr(host_, ExportedFunctions))
    {
        printf("Error! Failed to load plugin at path \"%s\". Plugin init function returned false. Check the logs to see if the plugin logged more detailed error messages.\n", copyPath.c_str());
        FreeLibrary(pluginHandle_);
        Active = false;
        return false;
    }
    else
        printf("Successfully initialized plugin \"%s\"\n", copyPath.c_str());

    Active = true;
    return true;
}

void Plugin::Shutdown()
{
    ShutdownFuncPtr();
    Active = false;
}

void Plugin::OnDependencyUnload(const string dependencyName)
{
    OnDependencyUnloadFuncPtr(dependencyName);
}

void Plugin::OnDependencyLoad(const string dependencyName)
{
    OnDependencyLoadFuncPtr(dependencyName);
}

bool Plugin::NeedsReload()
{
    //Return true if write time changed
    return pluginLastWriteTime_ != std::filesystem::last_write_time(path_);
}

bool Plugin::Reload()
{
    //Unload and reload the plugin
    Shutdown();
    Unload();
    return Load() && Init();
}

bool Plugin::DependsOn(const string& targetDependency)
{
    for (auto& dependency : Dependencies)
    {
        if (dependency == targetDependency)
            return true;
    }
    return false;
}