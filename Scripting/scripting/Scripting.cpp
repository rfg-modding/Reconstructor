#include "common/windows/WindowsWrapper.h"
#include "common/plugins/Plugin.h"
#include "rsl2/functions/Functions.h"
#include "rsl2/misc/GlobalState.h"
#include "common/Typedefs.h"
#include "common/Common.h"
#include "rsl2/IRSL2.h"
#include <pybind11/embed.h>
#include <cstdio>

IHost* host_ = nullptr;
IRSL2* rsl2_ = nullptr;

extern "C"
{
    //Called before init
    DLLEXPORT PluginInfo __cdecl RSL2_PluginInfo()
    {
        return PluginInfo
        {
            .Dependencies = { "RSL2" }
        };
    }

    //Called when the host dll loads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginInit(IHost* host, std::vector<PluginInterface>& exportedFunctions)
    {
        host_ = host;
        rsl2_ = (IRSL2*)host_->GetPluginInterface("RSL2", "RSL2");

        //Test out python
        pybind11::scoped_interpreter python{}; // start the interpreter and keep it alive
        pybind11::print("Hello, World!"); // use the Python API
        pybind11::exec(R"(
        kwargs = dict(name="World", number=42)
        message = "Hello, {name}! The answer is {number}".format(**kwargs)
        print(message)
        )");

        if (!rsl2_)
        {
            printf("Error! Failed to import all external functions in Scripting.dll::RSL2_PluginInit.\n");
            return false;
        }

        return true;
    }

    //Called when the host dll unloads this plugin
    DLLEXPORT bool __cdecl RSL2_PluginShutdown()
    {
        rsl2_ = nullptr;
        return true;
    }

    //Called immediately before dependency shutdown + unload
    DLLEXPORT void __cdecl RSL2_OnDependencyUnload(const string& dependencyName)
    {
        rsl2_ = nullptr;
    }

    //Called immediately after dependency load + init
    DLLEXPORT void __cdecl RSL2_OnDependencyLoad(const string& dependencyName)
    { 
        //Re-import external functions from dependency
        rsl2_ = (IRSL2*)host_->GetPluginInterface("RSL2", "RSL2");
    }
}