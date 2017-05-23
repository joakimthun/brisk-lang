#pragma once

#include <string>

#include "typedef.h"

namespace brisk {

	enum class TokenType : u16
	{
		// Operators
		Plus,
		Minus,
		Star,
		Slash,
		Equals,

		// Literals
		I32Literal,
		StrLiteral,

		// Keywords
		Fn,
		Ret,
		I32,
		Let,
		Mut,

		// Misc
		Identifier,
		LBracket,
		RBracket,
		LParen,
		RParen,
		RArrow,
		Comma,
		Eof
	};

	std::string token_type_name(TokenType type);
}