#include "register_allocator.h"

#include "brisk_exception.h"

namespace brisk {
	namespace x64 {

		RegisterAllocator::RegisterAllocator()
		{
			allocated_[static_cast<u8>(Register::EAX)] = false;
			allocated_[static_cast<u8>(Register::ECX)] = false;
			allocated_[static_cast<u8>(Register::EDX)] = false;
			allocated_[static_cast<u8>(Register::EBX)] = false;
			allocated_[static_cast<u8>(Register::ESP)] = false;
			allocated_[static_cast<u8>(Register::EBP)] = false;
			allocated_[static_cast<u8>(Register::ESI)] = false;
			allocated_[static_cast<u8>(Register::EDI)] = false;
			allocated_[static_cast<u8>(Register::R8)]  = false;
			allocated_[static_cast<u8>(Register::R9)]  = false;
			allocated_[static_cast<u8>(Register::R10)] = false;
			allocated_[static_cast<u8>(Register::R11)] = false;
			allocated_[static_cast<u8>(Register::R12)] = false;
			allocated_[static_cast<u8>(Register::R13)] = false;
			allocated_[static_cast<u8>(Register::R14)] = false;
			allocated_[static_cast<u8>(Register::R15)] = false;
		}

		Register RegisterAllocator::get_free()
		{
			// TODO: Handle running out of registers
			for (u8 i = 1; i < allocated_.size(); i++)
			{
				if (!allocated_[i])
					return static_cast<Register>(i);
			}

			throw BriskException("RegisterAllocator::get_free");
		}

		void RegisterAllocator::free(Register reg)
		{
			allocated_[static_cast<u8>(reg)] = false;
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
