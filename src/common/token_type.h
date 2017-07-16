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
		DoubleEquals,
		Not,
		NotEqual,
		BitOr,
		BitAnd,
		LogOr,
		LogAnd,

		// Literals
		IntLiteral,
		StrLiteral,
		FalseLiteral,
		TrueLiteral,

		// Keywords
		Fn,
		Ret,
		Let,
		Mut,
		Pkg,
		Ext,
		If,

		U8,
		I8,
		U16,
		I16,
		U32,
		I32,
		U64,
		I64,
		Float,
		Double,
		Void,
		Bool,

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
	std::string operator_from_token(TokenType type);
	bool is_boolean_operator(TokenType type);
}