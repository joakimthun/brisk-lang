#include "parsing_exception.h"

#include <sstream>

#include "file.h"
#include "token_type.h"

namespace brisk {

	ParsingException::ParsingException(TokenType expected, Token &actual)
	{
		const auto expected_name = token_type_name(expected);
		const auto actual_name = token_type_name(actual.type);

		std::ostringstream ss;
		ss << "Unexpected token" << std::endl << "File: '" << actual.file->path << "'" << std::endl;
		ss << "Row: " << actual.row << " Column: " << actual.column_start  << "-" << actual.column_end << std::endl;
		ss << "Expected: " << expected_name << " Actual: " << actual_name << std::endl;
		message_ = ss.str();
	}

	ParsingException::ParsingException(Token &token)
	{
		std::ostringstream ss;
		ss << "Unexpected token: " << token_type_name(token.type) << std::endl;
		ss << "File: '" << token.file->path << "'" << std::endl;
		ss << "Row: " << token.row << " Column: " << token.column_start << "-" << token.column_end << std::endl;
		message_ = ss.str();
	}

	ParsingException::ParsingException(const std::string &message) : BriskException(message)
	{
	}
}
