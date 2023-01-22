#include "CodePatcher.h"
#include "common/Defer.h"
#include <stdexcept>

void CodePatcher::Backup(const string& name, DWORD address, DWORD length, bool nop)
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

void CodePatcher::Restore(const string& name, bool removeFromCache)
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

void CodePatcher::Replace(DWORD address, std::span<u8> newValues)
{
	DWORD length = newValues.size_bytes();
	BYTE* byteAddress = reinterpret_cast<BYTE*>(address);
	DWORD originalProtectionPermissions;
	DWORD backup;

	VirtualProtect(byteAddress, length, PAGE_EXECUTE_READWRITE, &originalProtectionPermissions);
	for (DWORD i = 0x0; i < length; i++)
	{
		*(byteAddress + i) = newValues[i];
	}
	VirtualProtect(byteAddress, length, originalProtectionPermissions, &backup);
}

void CodePatcher::Replace(DWORD address, asmjit::CodeHolder& code)
{
	size_t codeSize = code.codeSize();
	if (codeSize == 0)
	{
		throw std::runtime_error("Passed empty code holder to CodePatcher::Replace()");
	}
	u8* bytes = new u8[codeSize];
	defer(delete[] bytes);

	code.flatten();
	code.copyFlattenedData(bytes, codeSize);
	std::span<u8> byteSpan(bytes, codeSize);
	Replace(address, byteSpan);
}

void CodePatcher::ReplaceAndBackup(const string& name, DWORD address, std::span<u8> newValues)
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