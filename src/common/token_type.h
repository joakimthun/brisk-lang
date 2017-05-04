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

}