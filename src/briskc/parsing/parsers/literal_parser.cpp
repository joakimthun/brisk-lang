#include "literal_parser.h"

#include "../brisk_parser.h"
#include "ast/ast.h"
#include "type.h"

namespace brisk {

	std::unique_ptr<Expr> LiteralParser::parse(BriskParser &parser)
	{
		auto expr = std::make_unique<LiteralExpr>();

		expr->start = parser.current_token();
		expr->end = parser.current_token();

		switch (parser.current_token().type)
		{
		case TokenType::I32Literal: {
			expr->type = Type::I32;
			expr->value.i32 = parser.current_token().value.i32;
			parser.consume(TokenType::I32Literal);
			break;
		}
		default:
			throw "LiteralParser::parse";
		}

		return expr;
	}

}