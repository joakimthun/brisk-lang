#include "emitter.h"

namespace brisk {
	namespace x64 {

		Emitter::Emitter()
		{
			buffer_ = std::make_unique<ByteBuffer>(1024);
		}

		void Emitter::emit_add(Register destination, Register source)
		{
			// 03 /r
			emit(0x03);
			emit_modrm(ModRM_Mod::RegisterAddr, source, destination);
		}

		void Emitter::emit_add(Register destination, u8 value)
		{
			// 83 /0 ib
			emit(0x83);
			emit_modrm(ModRM_Mod::RegisterAddr, destination);
			emit(value);
		}

		void Emitter::emit_add64(Register destination, u8 value)
		{
			// REX.W + 83 / 0 ib
			emit_rex(REX::W);
			emit(0x83);
			emit_modrm(ModRM_Mod::RegisterAddr, destination);
			emit(value);
		}

		void Emitter::emit_add64(Register destination, u32 value)
		{
			// REX.W + 81 /0 id
			emit_rex(REX::W);
			emit(0x81);
			emit_modrm(ModRM_Mod::RegisterAddr, destination);
			emit4(value);
		}

		void Emitter::emit_sub(Register destination, u8 value)
		{
			// 83 /5 ib
			emit(0x83);
			emit_modrm(ModRM_Mod::RegisterAddr, 0x5, (u8)destination);
			emit(value);
		}

		void Emitter::emit_sub64(Register destination, u8 value)
		{
			// REX.W + 83 /5 ib
			emit_rex(REX::W);
			emit(0x83);
			emit_modrm(ModRM_Mod::RegisterAddr, 0x5, (u8)destination);
			emit(value);
		}

		void Emitter::emit_sub64(Register destination, u32 value)
		{
			// REX.W + 81 / 5 id
			emit_rex(REX::W);
			emit(0x81);
			emit_modrm(ModRM_Mod::RegisterAddr, 0x5, (u8)destination);
			emit4(value);
		}

		void Emitter::emit_mov8(Register destination, u64 value)
		{
			// REX.W + B8 +rd io
			emit_rex(REX::W);
			emit(0xb8 | ((u8)destination));
			emit8(value);
		}

		void Emitter::emit_mov(Register destination, u32 value)
		{
			// B8+ rd id
			emit(0xb8 | ((u8)destination));
			emit4(value);
		}

		void Emitter::emit_mov(Register destination, Register source)
		{
			// 89 /r
			emit(0x89);
			emit_modrm(ModRM_Mod::RegisterAddr, source, destination);
		}

		void Emitter::emit_spd_mov1(u8 displacement, u8 value)
		{
			// C6 /0 ib
			emit(0xc6);
			emit_modrm(ModRM_Mod::Displacement1, 0, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
			emit(value);
		}

		void Emitter::emit_spd_mov2(u8 displacement, u16 value)
		{
			// C7 /0 iw
			emit(0x66); // Operand-size(word) override prefix
			emit(0xc7);
			emit_modrm(ModRM_Mod::Displacement1, 0, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
			emit2(value);
		}

		void Emitter::emit_spd_mov4(u8 displacement, u32 value)
		{
			// C7 /0 id
			emit(0xc7);
			emit_modrm(ModRM_Mod::Displacement1, 0, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
			emit4(value);
		}

		void Emitter::emit_spd_mov8(u8 displacement, Register source)
		{
			// REX.W + 89 /r
			emit_rex(REX::W);
			emit(0x89);
			emit_modrm(ModRM_Mod::Displacement1, source, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
		}

		void Emitter::emit_spd_movzx1(Register destination, u8 displacement)
		{
			// 0F B6 /r
			emit(0x0f);
			emit(0xb6);
			emit_modrm(ModRM_Mod::Displacement1, destination, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
		}

		void Emitter::emit_spd_movzx2(Register destination, u8 displacement)
		{
			// 0F B7 /r
			emit(0x0f);
			emit(0xb7);
			emit_modrm(ModRM_Mod::Displacement1, destination, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
		}

		void Emitter::emit_spd_movsx1(Register destination, u8 displacement)
		{
			// 0F BE /r
			emit(0x0f);
			emit(0xbe);
			emit_modrm(ModRM_Mod::Displacement1, destination, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
		}

		void Emitter::emit_spd_movsx2(Register destination, u8 displacement)
		{
			// 0F BF /r
			emit(0x0f);
			emit(0xbf);
			emit_modrm(ModRM_Mod::Displacement1, destination, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
		}

		void Emitter::emit_spd_mov(Register destination, u8 displacement)
		{
			//8B /r
			emit(0x8b);
			emit_modrm(ModRM_Mod::Displacement1, destination, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
		}

		void Emitter::emit_spd_mov(u8 displacement, Register source)
		{
			if (source > Register::RDI)
				emit_rex(REX::R);

			// 89 /r
			emit(0x89);
			emit_modrm(ModRM_Mod::Displacement1, source, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
		}

		void Emitter::emit_spd_mov64(Register destination, u8 displacement)
		{
			// REX.W + 8B / r
			emit_rex(REX::W);
			emit(0x8b);
			emit_modrm(ModRM_Mod::Displacement1, destination, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
		}

		void Emitter::emit_spd_mov64(u8 displacement, Register source)
		{
			// REX.W + 89 / r
			emit_rex(REX::W);
			emit(0x89);
			emit_modrm(ModRM_Mod::Displacement1, source, Register::RSP);
			emit_sib(SIBScale::X1, Register::RSP, Register::RSP);
			emit(displacement);
		}

		void Emitter::emit_lea64(Register destination, u32 displacement)
		{
			// REX.W + 8D /r
			emit_rex(REX::W);
			emit(0x8d);
			emit_modrm(ModRM_Mod::RegIndir, (u8)destination, 0x5);
			emit4(displacement);
		}

		void Emitter::emit_xor(Register destination, Register source)
		{
			emit(0x33);
			emit_modrm(ModRM_Mod::RegisterAddr, source, destination);
		}

		void Emitter::emit_ret()
		{
			// Near return to calling procedure.
			emit(0xc3);
		}

		void Emitter::emit_call()
		{
			// E8 cd
			emit(0xe8);
			emit4(0);
		}

		void Emitter::emit_rex(REX r)
		{
			u8 value = 0x40;
			switch (r)
			{
			case REX::W:
				value |= (1 << 3);
				break;
			case REX::R:
				value |= (1 << 2);
				break;
			case REX::X:
				value |= (1 << 1);
				break;
			case REX::B:
				value |= (1 << 0);
				break;
			default:
				break;
			}

			emit(value);
		}

		void Emitter::emit(u8 value)
		{
			buffer_->write(value);
		}

		void Emitter::emit2(u16 value)
		{
			auto byte_ptr = (u8*)&value;
			for (auto i = 0u; i < 2; i++)
			{
				emit(*byte_ptr);
				byte_ptr++;
			}
		}

		void Emitter::emit4(u32 value)
		{
			auto byte_ptr = (u8*)&value;
			for (auto i = 0u; i < 4; i++)
			{
				emit(*byte_ptr);
				byte_ptr++;
			}
		}

		void Emitter::emit8(u64 value)
		{
			auto byte_ptr = (u8*)&value;
			for (auto i = 0u; i < 8; i++)
			{
				emit(*byte_ptr);
				byte_ptr++;
			}
		}

		u32 Emitter::current_buffer_offset()
		{
			return buffer_->length();
		}

		std::unique_ptr<ByteBuffer> Emitter::buffer()
		{
			return std::move(buffer_);
		}

		void Emitter::emit_modrm(ModRM_Mod mod, Register rm)
		{
			emit_modrm(mod, 0, (u8)rm);
		}

		void Emitter::emit_modrm(ModRM_Mod mod, u8 rm)
		{
			emit_modrm(mod, 0, rm);
		}

		void Emitter::emit_modrm(ModRM_Mod mod, Register reg, Register rm)
		{
			emit_modrm(mod, (u8)reg, (u8)rm);
		}

		void Emitter::emit_modrm(ModRM_Mod mod, u8 reg, Register rm)
		{
			emit_modrm(mod, reg, (u8)rm);
		}

		void Emitter::emit_modrm(ModRM_Mod mod, u8 reg, u8 rm)
		{ 
			// 0x10 = 00.010.000 (mod = 0, reg / digit = 2, r / m = 0)
			// 0x11 = 00.010.001 (mod = 0, reg / digit = 2, r / m = 1).

			u8 result = rm | (reg << 3) | ((u8)mod << 6);
			buffer_->write(result);
		}

		void Emitter::emit_sib(SIBScale scale, Register index, Register base)
		{
			emit_sib(scale, (u8)index, (u8)base);
		}

		void Emitter::emit_sib(SIBScale scale, u8 index, u8 base)
		{
			u8 result = base | (index << 3) | ((u8)scale << 6);
			buffer_->write(result);
		}
	}
}
