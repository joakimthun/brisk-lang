#include "assignment_parser.h"

#include "../brisk_parser.h"
#include "ast/ast.h"

namespace brisk {

	std::unique_ptr<Expr> AssignmentParser::parse(BriskParser & parser, std::unique_ptr<Expr> left)
	{
		auto expr = std::make_unique<AssignExpr>();
		expr->start = parser.current_token();
		parser.consume(TokenType::Equals);
		expr->left = std::move(left);
		expr->right = parser.parse_expr(precedence());
		expr->end = parser.current_token();

		auto raw_expr_ptr = expr.get();
		parser.defer([raw_expr_ptr](auto &parser) {
			raw_expr_ptr->type = raw_expr_ptr->left->type;
		});

		return expr;
	}

	u8 AssignmentParser::precedence()
	{
		return static_cast<u8>(Precedence::Assignment);
	}

}
