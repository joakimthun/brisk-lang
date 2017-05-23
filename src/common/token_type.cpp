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
		case TokenType::Equals:
			return "Equals";
		case TokenType::I32Literal:
			return "I32Literal";
		case TokenType::StrLiteral:
			return "StrLiteral";
		case TokenType::Fn:
			return "Fn";
		case TokenType::Ret:
			return "Ret";
		case TokenType::I32:
			return "I32";
		case TokenType::Let:
			return "Let";
		case TokenType::Mut:
			return "Mut";
		case TokenType::Identifier:
			return "Identifier";
		case TokenType::LBracket:
			return "LBracket";
		case TokenType::RBracket:
			return "RBracket";
		case TokenType::LParen:
			return "LParen";
		case TokenType::RParen:
			return "RParen";
		case TokenType::RArrow:
			return "RArrow";
		case TokenType::Comma:
			return "Comma";
		default:
			return "Unknown token type";;
		}
	}

}
