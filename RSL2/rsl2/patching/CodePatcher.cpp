#include "CodePatcher.h"

void CodePatcher::BackupSnippet(const string& name, DWORD address, DWORD length, bool nop)
{
	BYTE* byteAddress = reinterpret_cast<BYTE*>(address);
	DWORD originalProtectionPermissions;
	DWORD backup;
	SnippetCache[name] = CodeSnippet(address, length);

	VirtualProtect(byteAddress, length, PAGE_EXECUTE_READWRITE, &originalProtectionPermissions);
	for (DWORD i = 0x0; i < length; i++)
	{
		SnippetCache[name].Opcodes.push_back(*(byteAddress + i));
		if (nop)
		{
			*(byteAddress + i) = 0x90;
		}
	}
	VirtualProtect(byteAddress, length, originalProtectionPermissions, &backup);
}

void CodePatcher::RestoreSnippet(const string& name, bool removeFromCache)
{
	BYTE* byteAddress = reinterpret_cast<BYTE*>(SnippetCache[name].Address);
	DWORD length = SnippetCache[name].Length;
	DWORD originalProtectionPermissions;
	DWORD backup;

	VirtualProtect(byteAddress, length, PAGE_EXECUTE_READWRITE, &originalProtectionPermissions);
	for (DWORD i = 0x0; i < length; i++)
	{
		*(byteAddress + i) = SnippetCache[name].Opcodes[i];
	}
	VirtualProtect(byteAddress, length, originalProtectionPermissions, &backup);
}

void CodePatcher::NOPSnippet(const string& name)
{
	BYTE* byteAddress = reinterpret_cast<BYTE*>(SnippetCache[name].Address);
	DWORD length = SnippetCache[name].Length;
	DWORD originalProtectionPermissions;
	DWORD backup;

	VirtualProtect(byteAddress, length, PAGE_EXECUTE_READWRITE, &originalProtectionPermissions);
	for (DWORD i = 0x0; i < length; i++)
	{
		*(byteAddress + i) = 0x90;
	}
	VirtualProtect(byteAddress, length, originalProtectionPermissions, &backup);
}

void CodePatcher::ReplaceSnippet(const string& name, DWORD address, std::span<u8> newValues)
{
	DWORD length = newValues.size_bytes();
	BYTE* byteAddress = reinterpret_cast<BYTE*>(address);
	DWORD originalProtectionPermissions;
	DWORD backup;
	SnippetCache[name] = CodeSnippet(address, length);

	VirtualProtect(byteAddress, length, PAGE_EXECUTE_READWRITE, &originalProtectionPermissions);
	for (DWORD i = 0x0; i < length; i++)
	{
		SnippetCache[name].Opcodes.push_back(*(byteAddress + i));
		*(byteAddress + i) = newValues[i];
	}
	VirtualProtect(byteAddress, length, originalProtectionPermissions, &backup);
}