#include "common/windows/WindowsWrapper.h"
#include "Host.h"

DWORD WINAPI HostThread(HMODULE hModule);
void OpenConsoleWindow();
void CloseConsoleWindow();

BOOL WINAPI DllMain(HINSTANCE dllHandle, DWORD reason, LPVOID reserved)
{
    //Reminder for later use if needed
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        OpenConsoleWindow();
        CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(HostThread), dllHandle, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        CloseConsoleWindow();
        break;
    default:
        break;
    }

    return TRUE;
}

DWORD WINAPI HostThread(HMODULE hModule)
{
    Host host;
    host.Run(hModule);
    return 0;
}

void OpenConsoleWindow()
{
    AllocConsole();
    FILE* pFile = NULL;
    freopen_s(&pFile, "CONOUT$", "r+", stdout);
}

void CloseConsoleWindow()
{
    FreeConsole();
}