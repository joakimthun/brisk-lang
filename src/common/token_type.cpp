#include "token_type.h"

namespace brisk {

	std::string token_type_name(TokenType type)
	{
		switch (type)
		{
		case TokenType::Eof:
			return "Eof";
		case TokenType::Plus:
			return "Plus";
		case TokenType::Minus:
			return "Minus";
		case TokenType::Star:
			return "Star";
		case TokenType::Slash:
			return "Slash";
		case TokenType::I32Literal:
			return "I32Literal";
		default:
			return "Unknown token type";;
		}
	}

}
