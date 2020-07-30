#include "Plugin.h"
#include "common/filesystem/Path.h"

//Todo: Move to helper class/file
std::string GetLastWin32ErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

Plugin::Plugin(const string& path)
{
    path_ = path;
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
    InitFuncPtr = reinterpret_cast<PluginInitFuncPtr>(GetProcAddress(pluginHandle_, "RSL2_PluginInit"));
    UnloadFuncPtr = reinterpret_cast<PluginUnloadFuncPtr>(GetProcAddress(pluginHandle_, "RSL2_PluginUnload"));

    //Ensure the plugin dll has all the interface functions
    if (!InitFuncPtr || !UnloadFuncPtr)
    {
        printf("Error! Failed to load plugin at path \"%s\". Plugin dll is not exporting the required interface functions.\n", copyPath.c_str());
        FreeLibrary(pluginHandle_);
        return false;
    }

    //Call the init function and make sure it returns success (true)
    if (!InitFuncPtr())
    {
        printf("Error! Failed to load plugin at path \"%s\". Plugin init function returned false. Check the logs to see if the plugin logged more detailed error messages.\n", copyPath.c_str());
        FreeLibrary(pluginHandle_);
        return false;
    }

    //Plugin sucessfully loaded
    return true;
}

void Plugin::Unload()
{
    //Call unload interface function so plugin can detach hooks, free allocations, etc
    UnloadFuncPtr();
    FreeLibrary(pluginHandle_);
}

bool Plugin::NeedsReload()
{
    //Return true if write time changed
    return pluginLastWriteTime_ != std::filesystem::last_write_time(path_);
}

bool Plugin::Reload()
{
    //Unload and reload the plugin
    Unload();
    return Load();
}