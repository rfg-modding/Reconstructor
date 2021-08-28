#include "common/windows/WindowsWrapper.h"
#include "common/filesystem/Path.h"
#include "common/timing/Timer.h"
#include <cstdio>
#include <filesystem>
#include <TlHelp32.h>
#include "Host.h"

void Host::Run(HINSTANCE hModule)
{
    //Todo: Use an actual logger
    printf("Host thread started.\n");
    printf("Loading plugin dlls...\n");
    InitialPluginLoad();

    printf("Done loading plugin dlls. Resuming RFGR main thread...\n");
    ResumeRfgMainThread(hModule);

    //Loop through plugins periodically and reload any that need to be reloaded
    printf("RFG main thread resumed. Host thread will now perform periodic plugin change checks and reload them when necessary.\n");
    u32 pluginChangeCheckDelayMs = 1000u; //Time between plugin reload checks in ms (1000ms = 1s)
    Timer pluginChangeTimer;
    while (true)
    {
        //Timer for plugin reload checks
        if (pluginChangeTimer.ElapsedMilliseconds() < pluginChangeCheckDelayMs)
        {
            Sleep(pluginChangeCheckDelayMs - pluginChangeTimer.ElapsedMilliseconds());
            continue;
        }

        //Loop through each plugin, reload if necessary
        auto pluginIterator = Plugins.begin();
        while (pluginIterator != Plugins.end())
        {
            if (pluginIterator->NeedsReload())
            {
                PerformingReload = true;
                std::vector<HANDLE> pausedThreads = PauseAllThreadsExceptCurrent();
                bool reloadResult = ReloadPlugin(*pluginIterator);
                ResumeThreads(pausedThreads);
                PerformingReload = false;
                if (reloadResult)
                {
                    printf("Reloaded %s!\n", Path::GetFileName(pluginIterator->Path()).c_str());
                }
                else
                {
                    printf("Failed to reload %s. Removing it from the plugin list.\n", Path::GetFileName(pluginIterator->Path()).c_str());
                    pluginIterator = Plugins.erase(pluginIterator);
                    continue;
                }
            }
            pluginIterator++;
        }

        pluginChangeTimer.Reset();
    }

    //Exit
    printf("Host thread exiting...\n");
    FreeLibraryAndExitThread(hModule, 0);
}

void* Host::GetPluginInterface(const string& pluginName, const string& functionName)
{
    for (auto& plugin : Plugins)
    {
        if (plugin.Name() != pluginName)
            continue;

        for (auto& pluginFunc : plugin.ExportedFunctions)
        {
            if (pluginFunc.Name == functionName)
                return pluginFunc.Ptr;
        }
    }

    return nullptr;
}

void Host::InitialPluginLoad()
{
    //Load all plugin DLLs and get their dependency info, but don't initialize them yet
    for (const auto& entry : std::filesystem::directory_iterator(pluginsPath))
    {
        //Skip folders and non .dll files
        if (!entry.path().has_extension())
            continue;
        if (entry.path().extension() != ".dll")
            continue;
        if (OnIgnoreList(Path::GetFileNameNoExtension(entry.path())))
            continue;

        //Create plugin. Load it. Add to Plugins if it loads correctly
        Plugin plugin(entry.path().string(), this);
        if (plugin.Load())
            Plugins.push_back(plugin);
    }

    //Todo: See if using a graph would simplify the dependency handling code
    //Initialize all plugins and their dependencies
    for (auto& plugin : Plugins)
        InitializePluginAndDependencies(plugin);

    //Remove plugins that failed to load
    auto pluginIter = Plugins.begin();
    while (pluginIter != Plugins.end())
    {
        if (pluginIter->Remove)
            pluginIter = Plugins.erase(pluginIter);

        pluginIter++;
    }
}

bool Host::OnIgnoreList(const string& dllNameNoExt)
{
    //Ignore dlls that contain the substring "copy"
    if (dllNameNoExt.find("copy") != string::npos) //Todo: Make into helper functions in String:: namespace
        return true;

    //Ignore dlls with names on the ignore list
    for (const auto& ignoredName : PluginIgnoreList)
    {
        if (dllNameNoExt == ignoredName)
            return true;
    }

    return false;
}

Plugin* Host::GetPlugin(const string& name)
{
    for (auto& plugin : Plugins)
    {
        if (plugin.Name() == name)
            return &plugin;
    }

    return nullptr;
}

bool Host::InitializePluginAndDependencies(Plugin& target)
{
    //If plugin has no dependencies, initialize it
    if (target.Dependencies.size() == 0)
    {
        if (target.Active)
            return true;
        else
            return target.Init();
    }

    //Otherwise we have to make sure it's dependencies are initialized first
    for (string& dependencyName : target.Dependencies)
    {
        //Make sure the dependency is present in the plugins folder
        Plugin* dependency = GetPlugin(dependencyName);
        if (!dependency)
        {
            printf("Error! Failed to find dependency \"%s\" for plugin \"%s\"\n", dependencyName.c_str(), target.Name().c_str());
            return false;
        }

        //Init dependency if not already done
        if (!dependency->Active)
        {
            //This will initialize the dependencies of this dependency if it has any
            bool dependencyInitResult = InitializePluginAndDependencies(*dependency);
            if (!dependencyInitResult)
                return false;
        }
    }

    //Dependencies are initialized, init our target
    bool targetInitResult = target.Init();
    if (!targetInitResult)
    {
        printf("Error! Failed to init plugin \"%s\"\n", target.Name().c_str());
        target.Remove = true;
    }
    return targetInitResult;
}

bool Host::ReloadPlugin(Plugin& target)
{
    //Find all plugins which depend on this one. We'll need to notify them when it's unloaded and then loaded
    std::vector<Plugin*> dependentPlugins;
    for (auto& plugin : Plugins)
        if (plugin.DependsOn(target.Name()))
            dependentPlugins.push_back(&plugin);

    //Notify all plugins who depend on this one that it's being unloaded
    for (auto* dependent : dependentPlugins)
        dependent->OnDependencyUnload(target.Name());

    //Reload the target
    bool reloadResult = target.Reload();
    //If the target fails to reload then unload all of it's dependents
    if (!reloadResult)
    {
        printf("Error! Failed to reload plugin \"%s\". Unloading it and it's dependents.\n", target.Name().c_str());
        UnloadPluginAndDependents(target);
    }
    //Else notify all dependent plugins that the target been loaded
    for (auto* dependent : dependentPlugins)
        dependent->OnDependencyLoad(target.Name());

    return true;
}

void Host::UnloadPluginAndDependents(Plugin& target)
{
    //Find all plugins which depend on this one
    std::vector<Plugin*> dependentPlugins;
    for (auto& plugin : Plugins)
        if (plugin.DependsOn(target.Name()))
            dependentPlugins.push_back(&plugin);

    //Unload plugins which depend on the target
    for (auto* dependent : dependentPlugins)
        UnloadPluginAndDependents(*dependent);

    //Unload the target
    target.Shutdown();
    target.Unload();
}

void Host::ResumeRfgMainThread(HINSTANCE hModule)
{
    //Find main thread of rfg.exe and resume it so the game runs
    DWORD PID = GetCurrentProcessId();
    const DWORD hostThreadId = GetThreadId(hModule);
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h != INVALID_HANDLE_VALUE)
    {
        THREADENTRY32 te;
        te.dwSize = sizeof(te);
        if (Thread32First(h, &te))
        {
            do
            {
                if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
                {
                    if (te.th32OwnerProcessID == PID && te.th32ThreadID != hostThreadId)
                    {
                        HANDLE ThreadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
                        if (ThreadHandle)
                            ResumeThread(ThreadHandle);
                    }
                }
                te.dwSize = sizeof(te);
            } while (Thread32Next(h, &te));
        }
        CloseHandle(h);
    }
    else
    {
        printf("Failed to get toolhelp 32 snapshot!\n");
    }
}

std::vector<HANDLE> Host::PauseAllThreadsExceptCurrent()
{
    const DWORD currentThreadId = GetCurrentThreadId();
    const DWORD PID = GetProcessId(NULL);
    std::vector<HANDLE> pausedThreads = {};

    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h == INVALID_HANDLE_VALUE)
        return {};

    THREADENTRY32 te;
    te.dwSize = sizeof(te);
    if (Thread32First(h, &te))
    {
        do
        {
            if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID))
            {
                if (te.th32OwnerProcessID == PID && te.th32ThreadID != currentThreadId)
                {
                    HANDLE threadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, te.th32ThreadID);
                    if (threadHandle)
                    {
                        pausedThreads.push_back(threadHandle);
                        SuspendThread(threadHandle);
                    }
                }
            }
        } while (Thread32Next(h, &te));
    }
    CloseHandle(h);

    return pausedThreads;
}

void Host::ResumeThreads(const std::vector<HANDLE>& threads)
{
    for (auto& thread : threads)
    {
        ResumeThread(thread);
    }
}
