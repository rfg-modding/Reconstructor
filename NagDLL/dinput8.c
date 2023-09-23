#include "dinputproxy.h"

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason,LPVOID v)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		char szPath[MAX_PATH];

		if (!GetSystemDirectory(szPath, sizeof(szPath)))
			return FALSE;

		strcat(szPath, "\\dinput8.dll");
		
		hLThis = hInst;
		hL = LoadLibrary(szPath);

		if (!hL) 
			return FALSE;

		p[0] = GetProcAddress(hL,"DirectInput8Create");
		p[1] = GetProcAddress(hL,"DllCanUnloadNow");
		p[2] = GetProcAddress(hL,"DllGetClassObject");
		p[3] = GetProcAddress(hL,"DllRegisterServer");
		p[4] = GetProcAddress(hL,"DllUnregisterServer");

		LPSTR commandLineArgs = GetCommandLineA();
		BOOL containsArg = (strstr(commandLineArgs, "/RanWithReconstructor") != NULL);
		if (containsArg == 0)
		{
			MessageBoxA(NULL, "Please run the game using SyncFaction or Launcher.exe.\r\nIf you run it directly or through Steam/GOG you won't have important bugfixes and improvements required by modern RFG mods.\r\nThe game will now close.", "Game not launched correctly", MB_OK);
			ExitProcess(0);
			return;
		}
	}

	if (reason == DLL_PROCESS_DETACH)
	{
		FreeLibrary(hL);
	}

	return TRUE;
}

// DirectInput8Create
//extern "C" __declspec(naked) void __stdcall __E__0__()
HRESULT __stdcall  __E__0__(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter)
{
	//__asm { jmp p[0*4]; }
	
	proxy_IDirectInput *proxyDI;
	IDirectInput8 *di;

	typedef HRESULT (__stdcall *pCreate)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);
	pCreate ppC = (pCreate)p[0*4];
	HRESULT ret = ppC(hinst, dwVersion, riidltf, &di, punkOuter);

	if (ret != DI_OK)
		return ret;

	proxyDI = (proxy_IDirectInput *)malloc(sizeof(proxy_IDirectInput));
    proxyDI->vtbl.QueryInterface = proxy_IDirectInput_QueryInterface;
    proxyDI->vtbl.AddRef = proxy_IDirectInput_AddRef;
    proxyDI->vtbl.Release = proxy_IDirectInput_Release;
    proxyDI->vtbl.CreateDevice = proxy_IDirectInput_CreateDevice;
    proxyDI->vtbl.EnumDevices = proxy_IDirectInput_EnumDevices;
    proxyDI->vtbl.GetDeviceStatus = proxy_IDirectInput_GetDeviceStatus;
    proxyDI->vtbl.RunControlPanel = proxy_IDirectInput_RunControlPanel;
    proxyDI->vtbl.Initialize = proxy_IDirectInput_Initialize;
    proxyDI->lpVtbl = &proxyDI->vtbl;
    proxyDI->di = di;
    proxyDI->refcount = 1;
    
    *ppvOut = (LPDIRECTINPUT)proxyDI;
		
	return ret; /* */
}

// DllCanUnloadNow
__declspec(naked) void __stdcall __E__1__()
{
	__asm
	{
		jmp p[1*4];
	}
}

// DllGetClassObject
__declspec(naked) void __stdcall __E__2__()
{
	__asm
	{
		jmp p[2*4];
	}
}

// DllRegisterServer
__declspec(naked) void __stdcall __E__3__()
{
	__asm
	{
		jmp p[3*4]; 
	}
}

// DllUnregisterServer
__declspec(naked) void __stdcall __E__4__()
{
	__asm 
	{ 
		jmp p[4*4]; 
	}
}

