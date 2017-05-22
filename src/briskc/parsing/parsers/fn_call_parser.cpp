#include "fn_call_parser.h"

#include "ast/ast.h"
#include "../brisk_parser.h"

namespace brisk {

	std::unique_ptr<Expr> FnCallParser::parse(BriskParser &parser)
	{
		auto expr = std::make_unique<FnCallExpr>();
		expr->start = parser.current_token();
		expr->name = parser.current_token().raw_value;
		parser.consume(TokenType::Identifier);
		parser.consume(TokenType::LParen);

		while (parser.current_token().type != TokenType::RParen)
		{
			expr->args.push_back(parser.parse_expr());
			if (parser.current_token().type == TokenType::Comma)
				parser.consume(TokenType::Comma);
		}

		expr->end = parser.current_token();
		parser.consume(TokenType::RParen);

		return expr;
	}

}
