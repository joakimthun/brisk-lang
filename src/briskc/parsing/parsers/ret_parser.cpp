#include "ret_parser.h"

#include "ast/ast.h"
#include "../brisk_parser.h"

namespace brisk {

	std::unique_ptr<Expr> RetParser::parse(BriskParser & parser)
	{
		auto expr = std::make_unique<RetExpr>();
		expr->owner = parser.current_fn_context();

		parser.consume(TokenType::Ret);

		expr->start = parser.current_token();
		expr->expr = parser.parse_expr();
		expr->end = parser.current_token();

		expr->type = expr->expr->type;

		return expr;
	}

}
