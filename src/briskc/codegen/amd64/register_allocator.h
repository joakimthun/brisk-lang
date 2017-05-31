#pragma once

#include <array>
#include <stack>

#include "typedef.h"
#include "x64.h"

namespace brisk {
	namespace x64 {

		enum class RegisterAllocatorMode
		{
			None = 0,
			Call = 1
		};

		class RegisterAllocator
		{
		public:
			RegisterAllocator();

			Register get_free();
			void free(Register reg);
			void prepare_call();
			void end_call_prepare();

			Register pop();
			void push(Register reg);
		private:
			std::array<bool, static_cast<u8>(Register::R15) + 1> allocated_;
			std::stack<Register> eval_stack_;
			RegisterAllocatorMode mode_;
		};

	}
}