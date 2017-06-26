#include "stack_allocator.h"

namespace brisk {
	namespace x64 {

		StackAllocator::StackAllocator(Emitter &emitter)
			:
			emitter_(emitter),
			// TODO: Account for stack parameters
			current_sp_offset_(40)
		{
		}

		void StackAllocator::reset()
		{
			current_sp_offset_ = 40;
		}

		void StackAllocator::reserve(FnDeclExpr &expr)
		{
			auto shadow_space = calc_shadow_space(expr);

			// TODO: Calculate the actual size needed
			emitter_.emit_sub64(x64::Register::RSP, 100 + shadow_space);
		}

		void StackAllocator::free(FnDeclExpr &expr)
		{
			auto shadow_space = calc_shadow_space(expr);
			emitter_.emit_add8(x64::Register::RSP, 100 + shadow_space);
		}

		sp_rel_addr StackAllocator::reserve(u32 num_bytes)
		{
			auto sp_offset = current_sp_offset_;
			current_sp_offset_ += num_bytes;
			return sp_offset;
		}

		u32 StackAllocator::calc_shadow_space(FnDeclExpr &expr)
		{
			return 32;
		}

	}
}
