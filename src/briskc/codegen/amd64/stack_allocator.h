#pragma once

#include "emitter.h"
#include "ast/ast.h"

namespace brisk {
	namespace x64 {

		typedef u32 sp_rel_addr;

		class StackAllocator
		{
		public:
			StackAllocator(Emitter &emitter);

			void reset();
			void reserve(FnDeclExpr &expr);
			void free(FnDeclExpr &expr);
			sp_rel_addr reserve(u32 num_bytes);
		private:
			u32 calc_shadow_space(FnDeclExpr &expr);

			Emitter &emitter_;
			u32 current_sp_offset_;
		};

	}
}