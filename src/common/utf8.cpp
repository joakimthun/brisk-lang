#include "utf8.h"

#include "brisk_exception.h"
#include "util.h"

namespace brisk {
	namespace utf8 {

		/*
			https://en.wikipedia.org/wiki/UTF-8
			Num bytes	Bits	FCP		LCP			Byte 1		Byte 2		Byte 3		Byte 4
			1			7		U+0000	U+007F		0xxxxxxx
			2			11		U+0080	U+07FF		110xxxxx	10xxxxxx
			3			16		U+0800	U+FFFF		1110xxxx	10xxxxxx	10xxxxxx
			4			21		U+10000	U+10FFFF	11110xxx	10xxxxxx	10xxxxxx	10xxxxxx
		*/

		u8 read_num_bytes(u8 first_byte)
		{
			/*
			192 - 0xC0 - 110xxxxx
			224 - 0xE0 - 1110xxxx
			240 - 0xF0 - 11110xxx
			*/

			static u16 mask[] =
			{
				0xC0, 0xE0, 0xF0
			};

			if ((first_byte & mask[2]) == mask[2])
				return 4;
			if ((first_byte & mask[1]) == mask[1])
				return 3;
			if ((first_byte & mask[0]) == mask[0])
				return 2;

			return 1;
		}

		CodePoint read_code_point(const u8 *bytes, u64 length, u64 offset)
		{
			if (offset >= length)
				return CodePoint(0, 0, true);

			const auto first_byte = bytes[offset];
			if (first_byte == '\0')
				return CodePoint(0, 0, true);

			const auto num_bytes = read_num_bytes(first_byte);

			if (num_bytes == 1)
				return CodePoint(first_byte, 1);

			if (offset + num_bytes >= length)
				throw BriskException("utf8::read_code_point: Cannot read past length");

			if (num_bytes == 2)
				return CodePoint(bytes_to_u16(bytes[offset], bytes[offset + 1]), 2);

			if (num_bytes == 3)
				return CodePoint(bytes_to_u32(bytes[offset], bytes[offset + 1], bytes[offset + 2], 0), 3);

			return CodePoint(bytes_to_u32(bytes[offset], bytes[offset + 1], bytes[offset + 2], bytes[offset + 3]), 4);
		}

	}
}
