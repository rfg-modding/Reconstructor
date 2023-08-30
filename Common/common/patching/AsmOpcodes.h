//License note: Originally from DashFaction, may have been modified: https://github.com/rafalh/dashfaction
//DashFaction is licensed under MPL2: https://github.com/rafalh/dashfaction/blob/c2e1b1cb42b637f5876268d91e6a4edb4f4b2cdc/LICENSE.txt
//Reconstructor is also licensed under MPL2 to keep things simple.

#pragma once
#include <cstdint>

// Deprecated. Use AsmWriter.

namespace asm_opcodes
{

// clang-format off
constexpr std::uint8_t jae_rel_short  = 0x73;
constexpr std::uint8_t fadd           = 0xD8;
constexpr std::uint8_t call_rel_long  = 0xE8;
constexpr std::uint8_t jmp_rel_long   = 0xE9;
constexpr std::uint8_t jmp_rel_short  = 0xEB;
// clang-format on

}
