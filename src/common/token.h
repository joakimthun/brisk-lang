#pragma once

#include <string>

#include "typedef.h"
#include "token_type.h"

namespace brisk {

	struct File;

	union TokenValue
	{
		i32 i32;
		float f;
		double d;
	};

	struct Token
	{
		Token();
		Token(TokenType type, u8 *raw_value, u32 length, u32 row, u32 column_start, u32 column_end, File *file);
		Token(TokenType type, u8 *raw_value, u32 length, u32 row, u32 column_start, u32 column_end, TokenValue value, File *file);

		TokenType type;
		TokenValue value;
		u8 *raw_value;
		u32 length;
		u32 row;
		u32 column_start;
		u32 column_end;
		File *file;
	};
}

