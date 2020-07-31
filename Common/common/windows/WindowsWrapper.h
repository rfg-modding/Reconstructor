#pragma once
#include "common/Typedefs.h"

//Target windows 7 with win10 sdk
#include <winsdkver.h>
#define _WIN32_WINNT 0x0601
#include <SDKDDKVer.h>
#define NOIME
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>

std::string GetLastWin32ErrorAsString();