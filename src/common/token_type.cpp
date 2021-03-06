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
		case TokenType::Modulus:
			return "Modulus";
		case TokenType::Equals:
			return "Equals";
		case TokenType::DoubleEquals:
			return "DoubleEquals";
		case TokenType::Not:
			return "Not";
		case TokenType::NotEqual:
			return "NotEqual";
		case TokenType::BitOr:
			return "BitOr";
		case TokenType::BitAnd:
			return "BitAnd";
		case TokenType::LogOr:
			return "LogOr";
		case TokenType::LogAnd:
			return "LogAnd";
		case TokenType::IntLiteral:
			return "IntLiteral";
		case TokenType::StrLiteral:
			return "StrLiteral";
		case TokenType::FalseLiteral:
			return "FalseLiteral";
		case TokenType::TrueLiteral:
			return "TrueLiteral";
		case TokenType::Fn:
			return "Fn";
		case TokenType::Ret:
			return "Ret";
		case TokenType::Let:
			return "Let";
		case TokenType::Mut:
			return "Mut";
		case TokenType::Pkg:
			return "Pkg";
		case TokenType::Ext:
			return "Ext";
		case TokenType::If:
			return "if";
		case TokenType::I32:
			return "I32";
		case TokenType::Void:
			return "Void";
		case TokenType::Bool:
			return "Bool";
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
			return "token_type_name: Unknown token type";;
		}
	}

	std::string operator_from_token(TokenType type)
	{
		switch (type)
		{
		case TokenType::Plus:
			return "+";
		case TokenType::Minus:
			return "-";
		case TokenType::Star:
			return "*";
		case TokenType::Slash:
			return "/";
		case TokenType::Equals:
			return "=";
		default:
			return "operator_from_token: Unknown token type";
		}
	}

	bool is_boolean_operator(TokenType type)
	{
		switch (type)
		{
		case TokenType::DoubleEquals:
		case TokenType::NotEqual:
			return true;
		default:
			return false;
		}
	}

}
