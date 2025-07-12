#pragma once
#include "common/patching/FunHook.h"
#include "common/string/String.h"
#include "common/Typedefs.h"
#include "pugixml.hpp"
#include "sentry.h"
#include <filesystem>
#define SENTRY_DSN "https://813b13910cb32921cb43c85a1350e294@o4509619634176000.ingest.us.sentry.io/4509619648593920"

extern void CheckUserAnalyticsOptIn();
extern void InstallSentryHooks();
extern void RemoveSentryHooks();
extern bool UserOptedIntoAnalytics;

#if defined BUILD_WITH_SENTRY && defined SENTRY_DSN
extern FunHook<int __cdecl(int argc, char** argv)> keen_main_hook;
#endif