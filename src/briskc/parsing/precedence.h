#pragma once

#include "typedef.h"

namespace brisk {

	enum class Precedence : u8
	{
		Assignment = 1,		// =, +=, -=, /=
		LogOr = 2,			// ||
		LogAnd = 3,			// &&
		BitOr = 4,			// |
		BitAnd = 5,			// &
		Equality = 6,		// ==, !=
		Relational = 7,		// <, >, <=, >=
		Shift = 8,			// <<, >>
		Sum = 9,			// + , -
		Product = 10,		// * , /, %
		Unary = 11,			// ++/--
	};

}
