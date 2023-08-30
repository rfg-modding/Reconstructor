#pragma once
#include "common/windows/WindowsWrapper.h"
#include <asmjit/src/asmjit/x86.h>
#include "common/Typedefs.h"
#include <unordered_map>
#include <vector>
#include <span>

enum Opcodes : int
{
	NOP = 0x90,
	JMP_REL8 = 0xEB,
	PUSH = 0x68,
	LEA = 0x8D,
	MOV = 0xC7,
	RET = 0xC3
};

class CodeSnippet
{
public:
	CodeSnippet() = default;
	CodeSnippet(DWORD Address_, DWORD Length_) : Address(Address_), Length(Length_) { }

	DWORD Address = 0;
	DWORD Length = 0;
	std::vector <int> Opcodes;
};

//Used to disable, replace, patch, and restore code and memory
class CodePatcher
{
public:
	void Backup(const string& name, DWORD address, DWORD length, bool nop);
	void Restore(const string& name, bool removeFromCache);
	void Replace(DWORD address, std::span<u8> newValues);
	void Replace(DWORD address, asmjit::CodeHolder& code);
	void ReplaceAndBackup(const string& name, DWORD address, std::span<u8> newValues);

private:
	std::unordered_map <std::string, CodeSnippet> SnippetCache;
};