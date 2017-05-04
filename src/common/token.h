#pragma once

#include "typedef.h"
#include "token_type.h"

namespace brisk {

	union TokenValue
	{
		i32 i32;
		float f;
		double d;
	};

	struct Token
	{
		inline Token()
			:
			type(TokenType::Eof),
			value({ 0 }),
			raw_value(nullptr),
			length(0),
			row(0),
			column_start(0),
			column_end(0)
		{}

		inline Token(TokenType type, u8 *raw_value, u32 length, u32 row, u32 column_start, u32 column_end)
			:
			type(type),
			value({ 0 }),
			raw_value(raw_value),
			length(length),
			row(row),
			column_start(column_start),
			column_end(column_end)
		{}

		inline Token(TokenType type, u8 *raw_value, u32 length, u32 row, u32 column_start, u32 column_end, TokenValue value)
			:
			type(type),
			value(value),
			raw_value(raw_value),
			length(length),
			row(row),
			column_start(column_start),
			column_end(column_end)
		{}

		TokenType type;		
		TokenValue value;
		u8 *raw_value;
		u32 length;
		u32 row;
		u32 column_start;
		u32 column_end;
	};
}

