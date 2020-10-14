#pragma once
#include "common/plugins/Plugin.h"
#include "common/plugins/IHost.h"
#include <vector>

//Todo: Provide host interface for plugins to interact with. Uses:
//Todo:     - Query for list of plugins and info about them
//Todo:     - Grab functions exported from other plugins
//Todo:     - Trigger reload of self or other plugins
//Todo:     - Maybe save persistent state between reloads by allocating it on the heap and giving the host a pointer to that data temporarily

//Todo: (Maybe. Might need to do this for DLLs like Dx11 to load into rfg.exe. Necessary for patching D3D11Present) Patch tmain entrypoint to infinite loop so we can patch in piece.

class Host : IHost
{
public:
    void Run(HINSTANCE hModule);
    void* GetPluginFunction(const string& pluginName, const string& functionName);

private:
    void InitialPluginLoad();
    bool OnIgnoreList(const string& dllNameNoExt);
    Plugin* GetPlugin(const string& name);
    bool InitializePluginAndDependencies(Plugin& target);
    bool ReloadPlugin(Plugin& target);
    void UnloadPluginAndDependents(Plugin& target);
    void ResumeRfgMainThread(HINSTANCE hModule);
    std::vector<HANDLE> PauseAllThreadsExceptCurrent();
    void ResumeThreads(const std::vector<HANDLE>& threads);

    //Holds plugins loaded by the host dll
    std::vector<Plugin> Plugins;
    //Dll names that the host won't try to load as plugins 
    const std::vector<string> PluginIgnoreList =
    {
        //DLLs included with RFGR 
        "binkw32",
        "discord-rpc",
        "Galaxy",
        "GalaxyPeer",
        "GfeSDK",
        "libeay32",
        "ssleay32",
        "sw_api",

        //Non-plugin RSL2 DLLs
        "Profiler",
        "Host"
    };

    //Todo: Make this work for release builds without needing to manually edit the path
    //Todo: Stick this in a file that's loaded at runtime
//Path to look for plugin dlls. Special value in debug builds for convenience
#ifdef DEBUG_BUILD
    string pluginsPath = "C:\\Users\\moneyl\\source\\repos\\RSL2\\out\\build\\x86-Debug\\bin\\";
#elif defined DEBUG_BUILD_OPTIMIZED
    string pluginsPath = "C:\\Users\\moneyl\\source\\repos\\RSL2\\out\\build\\x86-Release\\bin\\";
#else
    string pluginsPath = "./RSL2/Plugins/";
#endif
};