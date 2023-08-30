#include "common/Typedefs.h"
#include "common/windows/WindowsWrapper.h"
#include <iostream>
#include <fstream>
#include <filesystem>

bool InjectHostDll(PROCESS_INFORMATION& processInfo);

int main()
{
    //Todo: Make a gui for config / options. Maybe use dear imgui + directx, or use win32
    //Todo: De-hardcode this
#ifdef DEBUG_BUILD
    string rfgrPath = "D:/GOG/Games/Red Faction Guerrilla Re-Mars-tered/rfg.exe";
#else
    string rfgrPath = "./rfg.exe";
#endif
    //Start RFGR in a suspended state to prepare it for injection
    PROCESS_INFORMATION processInfo;
    STARTUPINFO startupInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    //Start rfg.exe process in suspended state
    printf("Starting rfg.exe in suspended state. Path = '%s'\n", rfgrPath.c_str());
    BOOL result = CreateProcessA(rfgrPath.c_str(), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED,
        nullptr, std::filesystem::path(rfgrPath).parent_path().string().c_str(), &startupInfo, &processInfo);

    if (result)
    {
        printf("Started rfg.exe in a suspended state...\n");
    }
    else
    {
        printf("Failed to start process for rfg.exe!\n");
        printf("Press any key to close...\n");
        std::string dummy;
        std::cin >> dummy;
        return 1;
    }

    //Inject host dll which will do the rest
    if (!InjectHostDll(processInfo))
    {
        printf("Press any key to close...\n");
        std::string dummy;
        std::cin >> dummy;
        return 1;
    }

    return 0;
}

bool InjectHostDll(PROCESS_INFORMATION& processInfo)
{
    //Info needed to inject host.dll into rfg.exed
#ifdef DEBUG_BUILD
    string hostDllPath = std::filesystem::current_path().string() + "/Host.dll";
#else
    string hostDllPath = std::filesystem::current_path().string() + "/Reconstructor/Host.dll";
#endif
    long hostDllPathSize = hostDllPath.length() + 1;
    HANDLE handle = processInfo.hProcess;

    //Allocate space for Host.dll in rfg.exe
    printf("Allocating space for Host.dll...\n");
    LPVOID myAlloc = VirtualAllocEx(handle, NULL, hostDllPathSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!myAlloc)
        throw std::runtime_error("Error! Failed to allocate space for Host.dll in rfg.exe");

    printf("Loading Host.dll into rfg.exe...\n");
    i32 writeResult = WriteProcessMemory(handle, myAlloc, hostDllPath.c_str(), hostDllPathSize, 0);
    if (writeResult == 0)
        throw std::runtime_error("Error! Failed to write Host.dll to rfg.exe");

    //Start thread in rfg.exe to launch Host.dll DllMain
    printf("Starting host thread...\n");
    DWORD remoteThreadId;
    LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibraryA("kernel32"), "LoadLibraryA");
    HANDLE ThreadReturn = CreateRemoteThread(handle, NULL, 0, addrLoadLibrary, myAlloc, 0, &remoteThreadId);
    if (ThreadReturn != NULL)
    {
        printf("Started host thread. Launcher exiting.\n");
        return true;
    }
    else
    {
        printf("Error! Failed to create remote thread to launch Host.dll in rfg.exe\n");
        return false;
    }
}