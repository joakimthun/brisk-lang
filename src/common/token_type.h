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
		Let,
		Mut,
		Pkg,

		I32,
		Void,

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