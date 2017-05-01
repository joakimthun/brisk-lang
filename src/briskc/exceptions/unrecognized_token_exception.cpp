#include "unrecognized_token_exception.h"

#include <sstream>

namespace brisk {

	UnrecognizedTokenException::UnrecognizedTokenException(u32 token, u32 row, u32 column, const std::string &filepath)
	{
		std::ostringstream ss;
		ss << "Unrecognized token:" << std::endl << "File: '" << filepath << "'" << std::endl;
		ss << "Token: '" << static_cast<char>(token) << "' Row: " << row << " Column: " << column << std::endl;
		message_ = ss.str();
	}
}
