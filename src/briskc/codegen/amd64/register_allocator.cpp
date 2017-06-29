#include "register_allocator.h"

#include "brisk_exception.h"

namespace brisk {
	namespace x64 {

		constexpr auto arr_index(Register r)
		{
			return static_cast<u8>(r);
		}

		RegisterAllocator::RegisterAllocator()
			:
			mode_(RegisterAllocatorMode::None)
		{
			allocated_[arr_index(Register::RAX)] = true;
			allocated_[arr_index(Register::RCX)] = false;
			allocated_[arr_index(Register::RDX)] = true;
			allocated_[arr_index(Register::RBX)] = false;
			allocated_[arr_index(Register::RSP)] = true;
			allocated_[arr_index(Register::RBP)] = false;
			allocated_[arr_index(Register::RSI)] = false;
			allocated_[arr_index(Register::RDI)] = false;
			allocated_[arr_index(Register::R8)]  = false;
			allocated_[arr_index(Register::R9)]  = false;
			allocated_[arr_index(Register::R10)] = false;
			allocated_[arr_index(Register::R11)] = false;
			allocated_[arr_index(Register::R12)] = false;
			allocated_[arr_index(Register::R13)] = false;
			allocated_[arr_index(Register::R14)] = false;
			allocated_[arr_index(Register::R15)] = false;
		}

		Register RegisterAllocator::get_free()
		{
			if (mode_ == RegisterAllocatorMode::Call)
			{
				Register reg;

				if (!allocated_[arr_index(Register::RCX)])
					reg = Register::RCX;
				else if (!allocated_[arr_index(Register::RDX)])
					reg = Register::RDX;
				else if (!allocated_[arr_index(Register::R8)])
					reg = Register::R8;
				else if (!allocated_[arr_index(Register::R9)])
					reg = Register::R9;
				else
					throw BriskException("RegisterAllocator::get_free: Need to handle stack args...");

				allocated_[arr_index(reg)] = true;
				return reg;
			}

			// TODO: Handle running out of registers
			for (u8 i = 0; i < allocated_.size(); i++)
			{
				if (!allocated_[i])
				{
					allocated_[i] = true;
					return static_cast<Register>(i);
				}
			}

			throw BriskException("RegisterAllocator::get_free");
		}

		void RegisterAllocator::free(Register reg)
		{
			allocated_[arr_index(reg)] = false;
		}

		void RegisterAllocator::prepare_call()
		{
			mode_ = RegisterAllocatorMode::Call;

			// TODO: Handle if some/all of these are in use
			allocated_[arr_index(Register::RCX)] = false;
			allocated_[arr_index(Register::RDX)] = false;
			allocated_[arr_index(Register::R8)] = false;
			allocated_[arr_index(Register::R9)] = false;
		}

		void RegisterAllocator::end_call_prepare()
		{
			mode_ = RegisterAllocatorMode::None;
		}

		Register RegisterAllocator::pop()
		{
			auto top = eval_stack_.top();
			eval_stack_.pop();
			return top;
		}

		void RegisterAllocator::push(Register reg)
		{
			eval_stack_.push(reg);
		}

	}
}
