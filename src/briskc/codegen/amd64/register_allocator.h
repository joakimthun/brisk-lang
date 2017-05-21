#pragma once

#include <array>
#include <stack>

#include "x64.h"

namespace brisk {
	namespace x64 {

		class RegisterAllocator
		{
		public:
			RegisterAllocator();

			Register get_free();
			void free(Register reg);

			Register pop();
			void push(Register reg);
		private:
			std::array<bool, static_cast<u8>(Register::R15) + 1> allocated_;
			std::stack<Register> eval_stack_;
		};

	}
}