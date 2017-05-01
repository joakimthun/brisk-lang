#pragma once

#include "typedef.h"

namespace brisk {
	namespace utf8 {
		
		struct CodePoint
		{
			inline CodePoint(u32 value, u8 num_bytes, bool eof = false)
				:
				value(value),
				num_bytes(num_bytes),
				eof(eof)
			{}

			u32 value;
			u8 num_bytes;
			bool eof;
		};

		u8 read_num_bytes(u8 first_byte);
		CodePoint read_code_point(const u8 *bytes, u64 length, u64 offset);
	}
}
