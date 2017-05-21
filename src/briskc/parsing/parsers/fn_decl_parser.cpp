#include "fn_decl_parser.h"

#include "../brisk_parser.h"
#include "ast/ast.h"

namespace brisk {

	std::unique_ptr<Expr> FnDeclParser::parse(BriskParser &parser)
	{
		parser.consume(TokenType::Fn);

		auto expr = std::make_unique<FnDeclExpr>();
		expr->body = std::make_unique<BlockExpr>();

		expr->start = parser.current_token();

		expr->name = parser.current_token().raw_value;
		parser.consume(TokenType::Identifier);

		parser.consume(TokenType::LParen);
		parser.consume(TokenType::RParen);

		parser.consume(TokenType::RArrow);
		parser.consume(TokenType::I32);
		parser.consume(TokenType::LBracket);

		while(parser.current_token().type != TokenType::RBracket)
			expr->body->exprs.push_back(parser.parse_expr());

		parser.consume(TokenType::RBracket);
		expr->end = parser.current_token();

		return expr;
	}

}
