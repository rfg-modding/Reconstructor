#include "DevToolsWndProc.h"
#include "devTools/misc/GlobalState.h"
#include "reconstructor/IReconstructor.h"
#include "reconstructor/misc/GlobalState.h"
#include "devTools/misc/GlobalState.h"

void __stdcall DevToolsWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Reconstructor_GlobalState* state = reconstructor->GetGlobalState();
    if (!state->ImGuiInitialized)
        return;

    if (msg == WM_KEYDOWN)
    {
        if ((HIWORD(lParam) & KF_REPEAT) == 0) //Key not repeating. Only detects first WM_KEYDOWN signal until WM_KEYUP
        {
            switch (wParam)
            {
            case VK_NUMPAD1:
                reconstructor->ToggleHud();
                break;

            case VK_NUMPAD2:
                reconstructor->ToggleFog();
                break;

            case VK_NUMPAD3:
                reconstructor->ToggleFreeCamera();
                break;

            default:
                break;
            }
        }
    }
}