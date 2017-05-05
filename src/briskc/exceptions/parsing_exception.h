#pragma once

#include <string>

#include "typedef.h"
#include "brisk_exception.h"
#include "token.h"

namespace brisk {

	struct ParsingException : public BriskException
	{
		ParsingException(TokenType expected, Token &actual);
		ParsingException(Token &token);
	};
}
