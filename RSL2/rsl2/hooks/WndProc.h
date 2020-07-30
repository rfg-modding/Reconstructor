#pragma once
#include "common/windows/WindowsWrapper.h"

extern void LockMouse();
extern void UnlockMouse();
extern LRESULT __stdcall RSL2_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);