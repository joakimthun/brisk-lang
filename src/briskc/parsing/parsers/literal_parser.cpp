#include "literal_parser.h"

#include "../brisk_parser.h"
#include "ast/ast.h"
#include "type.h"

#include "exceptions.h"

namespace brisk {

	std::unique_ptr<Expr> LiteralParser::parse(BriskParser &parser)
	{
		auto expr = std::make_unique<LiteralExpr>();

		expr->start = parser.current_token();
		expr->end = parser.current_token();

		switch (parser.current_token().type)
		{
		case TokenType::I32Literal: {
			expr->type = Type(TypeID::I32);
			expr->value.i32 = parser.current_token().value.i32;
			parser.consume(TokenType::I32Literal);
			break;
		}
		case TokenType::StrLiteral: {
			expr->type = Type(TypeID::U8, true);
			expr->str_value = parser.current_token().raw_value;
			parser.consume(TokenType::StrLiteral);
			break;
		}
		default:
			throw BriskException("LiteralParser::parse -> Unsupported type");
		}

		return expr;
	}

}