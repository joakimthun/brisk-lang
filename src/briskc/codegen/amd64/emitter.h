#pragma once

#include <memory>

#include "x64.h"
#include "byte_buffer.h"


namespace brisk {
	namespace x64 {

		class Emitter
		{
		public:
			Emitter();

			void emit_add(Register destination, Register source);
			void emit_add(Register destination, u8 value);
			void emit_add64(Register destination, u8 value);
			void emit_sub(Register destination, u8 value);
			void emit_sub64(Register destination, u8 value);
			void emit_mov64(Register destination, u64 value);
			void emit_mov(Register destination, u32 value);
			void emit_mov(Register destination, Register source);
			void emit_lea64(Register destination, u32 displacement);
			void emit_xor(Register destination, Register source);
			void emit_ret();
			void emit_call();
			void emit_rex(REX r);
			void emit(u8 value);
			void emit4(u32 value);
			void emit8(u64 value);

			std::unique_ptr<ByteBuffer> buffer();
		private:
			void emit_modrm(ModRM_Mod mod, Register rm);
			void emit_modrm(ModRM_Mod mod, u8 rm);
			void emit_modrm(ModRM_Mod mod, Register reg, Register rm);
			void emit_modrm(ModRM_Mod mod, u8 reg, u8 rm);

			std::unique_ptr<ByteBuffer> buffer_;
		};

	}
}