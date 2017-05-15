#pragma once

#include "x64.h"
#include "byte_buffer.h"


namespace brisk {
	namespace x64 {

		class Emitter
		{
		public:
			Emitter();

			void emit_add(Register destination, u8 value);
			void emit_sub(Register destination, u8 value);
			void emit_mov(Register destination, u64 value);
			void emit_mov(Register destination, Register source);
			void emit_ret();
			void emit_rex(REX r);
			void emit(u8 value);
			void emit8(u64 value);

			const ByteBuffer &buffer() const;
		private:
			void emit_modrm(ModRM_Mod mod, Register rm);
			void emit_modrm(ModRM_Mod mod, u8 rm);
			void emit_modrm(ModRM_Mod mod, Register reg, Register rm);
			void emit_modrm(ModRM_Mod mod, u8 reg, u8 rm);

			ByteBuffer buffer_;
		};

	}
}