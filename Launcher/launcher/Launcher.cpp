#include "common/Typedefs.h"
#include "common/windows/WindowsWrapper.h"
#include <iostream>
#include <fstream>
#include <filesystem>

void InjectHostDll(PROCESS_INFORMATION& processInfo);

int main()
{
    //Todo: Make a gui for config / options. Maybe use dear imgui + directx, or use win32
    //Todo: De-hardcode this
    string rfgrPath = "G:/GOG/Games/Red Faction Guerrilla Re-Mars-tered/rfg.exe";

    //Start RFGR in a suspended state to prepare it for injection
    PROCESS_INFORMATION processInfo;
    STARTUPINFO startupInfo;
    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    BOOL result = CreateProcessA(rfgrPath.c_str(), nullptr, nullptr, nullptr, FALSE, CREATE_SUSPENDED,
        nullptr, std::filesystem::path(rfgrPath).parent_path().string().c_str(), &startupInfo, &processInfo);

    if (!result)
        throw std::exception("Error! Failed to create RFGR process!");

    //Inject host dll which will do the rest
    InjectHostDll(processInfo);

    return 0;
}

void InjectHostDll(PROCESS_INFORMATION& processInfo)
{
    //Info needed to inject host.dll into rfg.exed
    string hostDllPath = std::filesystem::current_path().string() + "/Host.dll";
    long hostDllPathSize = hostDllPath.length() + 1;
    HANDLE handle = processInfo.hProcess;

    //Allocate space for Host.dll in rfg.exe
    LPVOID myAlloc = VirtualAllocEx(handle, NULL, hostDllPathSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!myAlloc)
        throw std::runtime_error("Error! Failed to allocate space for Host.dll in rfg.exe");

    i32 writeResult = WriteProcessMemory(handle, myAlloc, hostDllPath.c_str(), hostDllPathSize, 0);
    if (writeResult == 0)
        throw std::runtime_error("Error! Failed to write Host.dll to rfg.exe");

    //Start thread in rfg.exe to launch Host.dll DllMain
    DWORD remoteThreadId;
    LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibraryA("kernel32"), "LoadLibraryA");
    HANDLE ThreadReturn = CreateRemoteThread(handle, NULL, 0, addrLoadLibrary, myAlloc, 0, &remoteThreadId);
    if (ThreadReturn == NULL)
        throw std::runtime_error("Error! Failed to create remote thread to launch Host.dll in rfg.exe");
}