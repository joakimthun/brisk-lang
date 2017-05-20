#pragma once

#include <string>

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
		Equals,

		// Literals
		I32Literal,

		// Keywords
		Fn,
		Ret,
		I32,

		// Misc
		Identifier,
		LBracket,
		RBracket,
		LParen,
		RParen,
		RArrow,
	};

	std::string token_type_name(TokenType type);
}