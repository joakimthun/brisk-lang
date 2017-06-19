#pragma once

#include <string>

#include "typedef.h"
#include "token_type.h"
#include "string_view.h"

namespace brisk {

	struct File;

	struct Token
	{
		Token();
		Token(TokenType type, u8 *raw_value, u32 length, u32 row, u32 column_start, u32 column_end, File *file);

		TokenType type;
		StringView raw_value;
		u32 row;
		u32 column_start;
		u32 column_end;
		File *file;
	};
}

