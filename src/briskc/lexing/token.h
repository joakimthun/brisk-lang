#pragma once

#include "typedef.h"

namespace brisk {

	enum class TokenType : u16
	{
		Eof = 0,

		// Operators
		Plus,
		Minus,
		Star,
		Slash,

		// Literals
		I32Literal
	};


	union TokenValue 
	{
		i32 i32;
		float f;
		double d;
	};

	struct Token
	{
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

