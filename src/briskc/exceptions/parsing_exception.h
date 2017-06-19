#pragma once

#include <string>

#include "typedef.h"
#include "brisk_exception.h"
#include "token.h"

namespace brisk {

	struct ParsingException : public BriskException
	{
		ParsingException(TokenType expected, const Token &actual);
		ParsingException(const Token &token);
		ParsingException(const std::string &message, const Token &token);
		ParsingException(const std::string &message);
	};
}
