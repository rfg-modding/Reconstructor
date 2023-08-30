#include "WndProc.h"
#include "common/Typedefs.h"
#include "reconstructor/misc/GlobalState.h"
#include "RFGR_Types/rfg/Camera.h"
#include "reconstructor/hooks/Camera.h"
#include "reconstructor/util/Util.h"
#include "reconstructor/functions/FunctionsInternal.h"
#include "RFGR_Types/rfg/Player.h"

//Functions for locking / unlocking the games auto-centering and hiding of the mouse. For imgui interaction with game running
bool MouseUnlocked = false;
char* MouseGenericPollOriginalCode = nullptr;
char* CenterMouseCursorOriginalCode = nullptr;

void LockMouse()
{
    if (!MouseUnlocked)
        return;

    static Reconstructor_GlobalState* globalState = GetGlobalState();
    globalState->Patcher.Restore("MouseGenericPollMouseVisible", true);
    globalState->Patcher.Restore("CenterMouseCursorCall", true); //Todo: See if this patch is even needed anymore

    printf("Locked mouse movement.\n");
    MouseUnlocked = false;
}
void UnlockMouse()
{
    if (MouseUnlocked)
        return;

    static Reconstructor_GlobalState* globalState = GetGlobalState();
    globalState->Patcher.Backup("MouseGenericPollMouseVisible", globalState->MouseGenericPollMouseVisibleAddress, 12, true);
    globalState->Patcher.Backup("CenterMouseCursorCall", globalState->CenterMouseCursorCallAddress, 11, true);

    MouseUnlocked = true;
    printf("Unlocked mouse movement.\n");
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT ProcessInput(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);// , const KeyState& keys);
extern LRESULT __stdcall Reconstructor_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static Reconstructor_GlobalState* globalState = GetGlobalState();

    //Pass input to Reconstructor input function
    ProcessInput(hWnd, msg, wParam, lParam);

    //Todo: Make it so you can have imgui visible without unlocking mouse look. Useful for data overlays
    if (globalState->GuiActive)
    {
        //When imgui is active only pass WM_SIZE messages to game so it doesn't react to gui mouse/keyboard input
        if (msg == WM_SIZE)
            return CallWindowProc(globalState->RfgWndProc, hWnd, msg, wParam, lParam);

        //Todo: Pass input to dear imgui when necessary
        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
        return true;
    }

    //If no blocking conditions met simply pass message to game WndProc
    return CallWindowProc(globalState->RfgWndProc, hWnd, msg, wParam, lParam);
}

LRESULT ProcessInput(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)// , const KeyState& keys);
{
    static Reconstructor_GlobalState* globalState = GetGlobalState();
    if (!globalState->ImGuiInitialized)
        return 0;

    f32 camSpeed = 5.0f;

    if (msg == WM_KEYDOWN)
    {
        if ((HIWORD(lParam) & KF_REPEAT) == 0) //Key not repeating. Only detects first WM_KEYDOWN signal until WM_KEYUP
        {
            switch (wParam)
            {
            //case VK_F1:
            //    globalState->GuiActive = !globalState->GuiActive;
            //    if (globalState->GuiActive)
            //        UnlockMouse();
            //    else
            //        LockMouse();
            //case VK_F7:
            //    ReloadXtbls();
            //    break;
            case VK_NUMPAD1:
                ToggleHud();
                break;
            case VK_NUMPAD2:
                ToggleFog();
                break;
            default:
                break;
            }
        }
    }

    //Pass input to camera code
    //CameraProcessInput(hwnd, msg, wParam, lParam);
}