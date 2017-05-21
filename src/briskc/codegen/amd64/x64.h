#pragma once

#include "typedef.h"

namespace brisk {
	namespace x64 {

		enum class Register : u8
		{
			EAX = 0,	RAX = 0,
			ECX = 1,	RCX = 1,
			EDX = 2,	RDX = 2,
			EBX = 3,	RBX = 3,
			ESP = 4,	RSP = 4,
			EBP = 5,	RBP = 5,
			ESI = 6,	RSI = 6,
			EDI = 7,	RDI = 7,
			R8 =  8,
			R9 =  9,
			R10 = 10,
			R11 = 11,
			R12 = 12,
			R13 = 13,
			R14 = 14,
			R15 = 15
		};

		// The MOD field specifies the addressing mode
		enum class ModRM_Mod : u8
		{
			RegIndir = 0,		// Register indirect addressing mode or SIB with no displacement (when R/M = 100) or Displacement only addressing mode (when R/M = 101).
			Displacement1 = 1,	// One-byte signed displacement follows addressing mode byte(s).
			Displacement4 = 2,	// Four-byte signed displacement follows addressing mode byte(s).
			RegisterAddr = 3	// Register addressing mode.
		};

		//  0   1   0   0 | W | R | X | B
		enum class REX : u8
		{
			W = 0,	// When 1, a 64-bit operand size is used. Otherwise, when 0, the default operand size is used (which is 32-bit for most but not all instructions)
			R = 1,	// This 1-bit value is an extension to the MODRM.reg field.
			X = 2,	// This 1-bit value is an extension to the SIB.index field.
			B = 3	// This 1-bit value is an extension to the MODRM.rm field or the SIB.base field.
		};
	}
}
