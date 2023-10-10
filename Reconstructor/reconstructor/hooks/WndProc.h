#pragma once
#include "common/windows/WindowsWrapper.h"

extern void LockMouse();
extern void UnlockMouse();
extern LRESULT __stdcall Reconstructor_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using WndProcCallbackFunction = void(__stdcall *)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern void AddWndProcCallback(WndProcCallbackFunction callback);