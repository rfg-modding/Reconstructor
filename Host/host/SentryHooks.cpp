#include "SentryHooks.h"
#include "common/BuildConfig.h"

bool UserOptedIntoAnalytics = false;

//int __cdecl keen::GameFramework::main(int argc, char **argv) //0x008109E0
FunHook<int __cdecl(int argc, char** argv)> keen_main_hook
{
    0x008109E0,
    [](int argc, char** argv)
    {
        //Always disable the default exception handler. I don't want people to see the old handler and wrongly think they have the new error reporting enabled.
        SetUnhandledExceptionFilter(NULL);

#if defined BUILD_WITH_SENTRY && defined SENTRY_DSN
        if (UserOptedIntoAnalytics)
        {
            //Setup sentry. Will override the exception handler set by RFG before it calls this function
            sentry_options_t* options = sentry_options_new();
            sentry_options_set_dsn(options, SENTRY_DSN);

            sentry_options_set_database_path(options, ".sentry-native");
            sentry_options_set_release(options, config::BuildVersion);
            sentry_options_set_debug(options, 1);
            sentry_init(options);
        }
#endif

        int result = keen_main_hook.CallTarget(argc, argv);

#if defined BUILD_WITH_SENTRY && defined SENTRY_DSN
        if (UserOptedIntoAnalytics)
        {
            //Game is closing. Shutdown sentry
            sentry_close();
        }
#endif

        return result;
    }
};

void InstallSentryHooks()
{
#if defined BUILD_WITH_SENTRY && defined SENTRY_DSN
    uintptr_t moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));
    CommonLib_ModuleBase = moduleBase;
    CheckUserAnalyticsOptIn();

    //Still installs the main hook even if they opt out so the default exception handler gets disabled. 
    //I don't want people to see the default popup and think they have sentry enabled when they don't. The old dump handling is defunct so it doesn't hurt anything to disable it.
    keen_main_hook.Install();
#endif
}

void RemoveSentryHooks()
{
#if defined BUILD_WITH_SENTRY && defined SENTRY_DSN
    keen_main_hook.Remove();
#endif
}

void CheckUserAnalyticsOptIn()
{
#if defined BUILD_WITH_SENTRY && defined SENTRY_DSN
    const char* configPath = "./Reconstructor/Config.xml";
    if (std::filesystem::exists(configPath))
    {
        pugi::xml_document doc;
        auto parseResult = doc.load_file(configPath);
        if (parseResult.status == pugi::status_ok)
        {
            auto root = doc.child("config");
            auto userConsentsNode = root.child("UserConsentsToAnalytics");
            UserOptedIntoAnalytics = userConsentsNode.text().as_bool();
        }
        else
        {
            MessageBox(NULL, "Reconstructor failed to parse it's config file. You should delete Config.xml in the Reconstructor folder and restart RFG to make sure everything works properly.", "Config parsing error", MB_OK);
        }
    }
    else
    {
        int result = MessageBoxA(NULL, "Do you consent to uploading analytics and crash report data for RFGR?\r\nDoing so will help modders find and fix bugs in RFGR.\r\nIt'd be greatly appreciated if you pressed yes.", "Do you consent to error reporting?", MB_YESNO);
        UserOptedIntoAnalytics = (result == IDYES);
        pugi::xml_document doc;
        auto root = doc.append_child("config");
        auto userConsentsNode = root.append_child("UserConsentsToAnalytics");
        userConsentsNode.text().set(UserOptedIntoAnalytics);
        doc.save_file(configPath);
    }

    if (UserOptedIntoAnalytics)
    {
        printf("Analytics enabled\n");
    }
    else
    {
        printf("Analytics disabled\n");
    }
#endif
}