#include "fn_decl_parser.h"

#include "../brisk_parser.h"
#include "ast/ast.h"

namespace brisk {

	std::unique_ptr<Expr> FnDeclParser::parse(BriskParser &parser)
	{
		parser.consume(TokenType::Fn);

		auto expr = std::make_unique<FnDeclExpr>(parser.current_scope());
		parser.push_scope(&expr->symbol_table);

		expr->start = parser.current_token();

		expr->name = parser.current_token().raw_value;
		parser.consume(TokenType::Identifier);

		parser.consume(TokenType::LParen);
		parser.consume(TokenType::RParen);

		parser.consume(TokenType::RArrow);

		// TODO: Validate this consume
		parser.consume();

		parser.consume(TokenType::LBracket);

		while(parser.current_token().type != TokenType::RBracket)
			expr->body.push_back(parser.parse_expr());

		parser.consume(TokenType::RBracket);
		expr->end = parser.current_token();

		parser.pop_scope();
		parser.current_scope()->add_fn(expr.get());

		return expr;
	}

}
