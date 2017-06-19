#include "token.h"

namespace brisk {

	Token::Token()
		:
		type(TokenType::Eof),
		row(0),
		column_start(0),
		column_end(0),
		file(nullptr)
	{}

	Token::Token(TokenType type, u8 *raw_value, u32 length, u32 row, u32 column_start, u32 column_end, File *file)
		:
		type(type),
		raw_value(StringView(raw_value, length)),
		row(row),
		column_start(column_start),
		column_end(column_end),
		file(file)
	{}
}
