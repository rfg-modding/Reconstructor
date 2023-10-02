#pragma once
#include "common/patching/FunHook.h"
#include "common/string/String.h"
#include "common/Typedefs.h"
#include "SentrySecrets.h"
#include "pugixml.hpp"
#include "sentry.h"
#include <filesystem>

extern void CheckUserAnalyticsOptIn();
extern void InstallSentryHooks();
extern void RemoveSentryHooks();
extern bool UserOptedIntoAnalytics;

#if defined BUILD_WITH_SENTRY && defined SENTRY_DSN
extern FunHook<int __cdecl(int argc, char** argv)> keen_main_hook;
#endif