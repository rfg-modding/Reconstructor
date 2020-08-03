#pragma once
#include "common/Typedefs.h"

//Host interface that plugins use to interact with other plugins
class IHost
{
public:
    virtual void* GetPluginFunction(const string& pluginName, const string& functionName) = 0;
};