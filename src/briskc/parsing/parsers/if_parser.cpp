#include "if_parser.h"

#include "../brisk_parser.h"
#include "ast/ast.h"
#include "../../types/type_util.h"

namespace brisk {

	std::unique_ptr<Expr> IfParser::parse(BriskParser &parser)
	{
		auto expr = std::make_unique<IfExpr>();
		expr->type = parser.type_table().get("void", false);
		expr->start = parser.current_token();
		parser.consume(TokenType::If);

		expr->if_condition = parser.parse_expr();

		parser.consume(TokenType::LBracket);
		expr->if_body_scope = std::make_unique<Block>(parser.current_scope());
		parser.push_scope(&expr->if_body_scope->symbol_table);

		while (parser.current_token().type != TokenType::RBracket)
			expr->if_body.push_back(parser.parse_expr());

		expr->end = parser.current_token();
		parser.consume(TokenType::RBracket);

		parser.pop_scope();

		return expr;
	}

}
