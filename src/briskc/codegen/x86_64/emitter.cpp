#include "emitter.h"

namespace brisk {
	namespace x64 {

		Emitter::Emitter()
		{
			buffer_ = std::make_unique<ByteBuffer>();
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

		void Emitter::emit_mov64(Register destination, u64 value)
		{
			// REX.W + B8 +rd io
			emit_rex(REX::W);
			emit(0xb8 | ((u8)destination));
			emit8(value);
		}

		void Emitter::emit_mov(Register destination, Register source)
		{
			// 89 /r
			emit(0x89);
			emit_modrm(ModRM_Mod::RegisterAddr, source, destination);
		}

		void Emitter::emit_lea64(Register destination, u32 displacement)
		{
			// REX.W + 8D /r
			emit_rex(REX::W);
			emit(0x8d);
			emit_modrm(ModRM_Mod::RegIndir, (u8)destination, 0x5);
			emit4(displacement);
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

		void Emitter::emit_modrm(ModRM_Mod mod, u8 reg, u8 rm)
		{ 
			// 0x10 = 00.010.000 (mod = 0, reg / digit = 2, r / m = 0)
			// 0x11 = 00.010.001 (mod = 0, reg / digit = 2, r / m = 1).

			u8 result = rm | ((u8)reg << 3) | ((u8)mod << 6);
			buffer_->write(result);
		}
	}
}
