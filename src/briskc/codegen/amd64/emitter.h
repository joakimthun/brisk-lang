#pragma once

#include <memory>

#include "x64.h"
#include "byte_buffer.h"


namespace brisk {
	namespace x64 {
		
		/* 
			Opcode Column

			NP — Indicates the use of 66 / F2 / F3 prefixes(beyond those already part of the instructions opcode) are not
			allowed with the instruction.Such use will either cause an invalid - opcode exception(#UD) or result in the
			encoding for a different instruction.

			REX.W — Indicates the use of a REX prefix that affects operand size or instruction semantics.The ordering of
			the REX prefix and other optional / mandatory instruction prefixes are discussed Chapter 2. Note that REX
			prefixes that promote legacy instructions to 64 - bit behavior are not listed explicitly in the opcode column.

			/digit — A digit between 0 and 7 indicates that the ModR / M byte of the instruction uses only the r / m(register
				or memory) operand.The reg field contains the digit that provides an extension to the instruction's opcode.

			/r — Indicates that the ModR / M byte of the instruction contains a register operand and an r / m operand.

			cb, cw, cd, cp, co, ct — A 1 - byte(cb), 2 - byte(cw), 4 - byte(cd), 6 - byte(cp), 8 - byte(co) or 10 - byte(ct) value
			following the opcode.This value is used to specify a code offset and possibly a new value for the code segment
			register.

			ib, iw, id, io — A 1 - byte(ib), 2 - byte(iw), 4 - byte(id) or 8 - byte(io) immediate operand to the instruction that
			follows the opcode, ModR / M bytes or scale - indexing bytes.The opcode determines if the operand is a signed
			value.All words, doublewords and quadwords are given with the low - order byte first.

			+rb, +rw, +rd, +ro — Indicated the lower 3 bits of the opcode byte is used to encode the register operand
			without a modR / M byte.The instruction lists the corresponding hexadecimal value of the opcode byte with low
			3 bits as 000b.In non - 64 - bit mode, a register code, from 0 through 7, is added to the hexadecimal value of the
			opcode byte.In 64 - bit mode, indicates the four bit field of REX.b and opcode[2:0] field encodes the register
			operand of the instruction.“ + ro” is applicable only in 64 - bit mode.See Table 3 - 1 for the codes.

			+i — A number used in floating - point instructions when one of the operands is ST(i) from the FPU register stack.
			The number i(which can range from 0 to 7) is added to the hexadecimal byte given at the left of the plus sign
			to form a single opcode byte.
		*/

		class Emitter
		{
		public:
			Emitter();

			void emit_add(Register destination, Register source);
			void emit_add(Register destination, u8 value);
			void emit_add64(Register destination, u8 value);
			void emit_add64(Register destination, u32 value);

			void emit_sub(Register destination, u8 value);
			void emit_sub64(Register destination, u8 value);
			void emit_sub64(Register destination, u32 value);

			void emit_mov8(Register destination, u64 value);
			void emit_mov(Register destination, u32 value);
			void emit_mov(Register destination, Register source);

			void emit_spd_mov1(u8 displacement, u8 value);
			void emit_spd_mov2(u8 displacement, u16 value);
			void emit_spd_mov4(u8 displacement, u32 value);
			void emit_spd_mov4(Register destination, u8 displacement);
			void emit_spd_mov8(u8 displacement, Register source);
			void emit_spd_mov8(Register destination, u8 displacement);

			void emit_spd_movzx1(Register destination, u8 displacement);
			void emit_spd_movzx2(Register destination, u8 displacement);

			void emit_spd_movsx1(Register destination, u8 displacement);
			void emit_spd_movsx2(Register destination, u8 displacement);

			void emit_lea64(Register destination, u32 displacement);

			void emit_xor(Register destination, Register source);

			void emit_ret();

			void emit_call();

			void emit_rex(REX r);
			void emit(u8 value);
			void emit2(u16 value);
			void emit4(u32 value);
			void emit8(u64 value);

			u32 current_buffer_offset();
			std::unique_ptr<ByteBuffer> buffer();

		private:
			void emit_modrm(ModRM_Mod mod, Register rm);
			void emit_modrm(ModRM_Mod mod, u8 rm);
			void emit_modrm(ModRM_Mod mod, Register reg, Register rm);
			void emit_modrm(ModRM_Mod mod, u8 reg, Register rm);
			void emit_modrm(ModRM_Mod mod, u8 reg, u8 rm);

			void emit_sib(SIBScale scale, Register index, Register base);
			void emit_sib(SIBScale scale, u8 index, u8 base);

			std::unique_ptr<ByteBuffer> buffer_;
		};

	}
}