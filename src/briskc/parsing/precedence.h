#pragma once

#include "typedef.h"

namespace brisk {

	enum class Precedence : u8
	{
		Assignment = 1,			// =, +=, -=, /=
		ConditionalOr = 2,		// ||
		ConditionalAnd = 3,		// &&
		LogicalOr = 4,			// |
		LogicalAnd = 5,			// &
		Equality = 6,			// ==, !=
		Relational = 7,			// <, >, <=, >=
		Shift = 8,				// <<, >>
		Sum = 9,				// + , -
		Product = 10,			// * , /, %
		Unary = 11,				// ++/--
	};

}
