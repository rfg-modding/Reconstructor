#pragma once
#include "common/Typedefs.h"

//Host interface that plugins use to interact with other plugins
class IHost
{
public:
    virtual void* GetPluginInterface(const string& pluginName, const string& interfaceName) = 0;

    //Todo: Come up with a better way of handling this. Maybe just wait til RFG hits it's main loop and freeze it there.
    //Plugins can check this to see if it's safe to do cross-dll things. Shouldn't do them during reloads
    bool PerformingReload = false;
};