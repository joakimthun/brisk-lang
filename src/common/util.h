#pragma once

#include <memory>
#include <string>

#include "typedef.h"

namespace brisk {

	#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr

	inline u16 bytes_to_u16(u8 first_byte, u8 second_byte)
	{
		return (u16)((first_byte) | (second_byte << 8));
	}

	inline u32 bytes_to_u32(u8 first_byte, u8 second_byte, u8 third_byte, u8 fourth_byte)
	{
		return (first_byte) | (second_byte << 8) | (third_byte << 16) | (fourth_byte << 24);
	}

	std::unique_ptr<u8> get_full_path_name(const std::string &path);

}
