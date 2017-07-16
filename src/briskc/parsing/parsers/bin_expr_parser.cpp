#include "bin_expr_parser.h"

#include "../brisk_parser.h"
#include "ast/ast.h"

namespace brisk {

	BinExprParser::BinExprParser(Precedence precedence)
		:
		precedence_(precedence)
	{
	}

	std::unique_ptr<Expr> BinExprParser::parse(BriskParser &parser, std::unique_ptr<Expr> left)
	{
		auto expr = std::make_unique<BinExpr>();
		expr->start = parser.current_token();

		expr->op = parser.current_token().type;
		parser.consume();
		expr->left = std::move(left);
		expr->right = parser.parse_expr(precedence());

		expr->end = parser.current_token();

		auto raw_expr_ptr = expr.get();
		parser.defer([raw_expr_ptr](auto &parser) {
			if (is_boolean_operator(raw_expr_ptr->op))
			{
				raw_expr_ptr->type = parser.type_table().get("bool", false);
			}
			else
			{
				raw_expr_ptr->type = raw_expr_ptr->left->type;
			}
		});

		return expr;
	}

	u8 BinExprParser::precedence()
	{
		return static_cast<u8>(precedence_);
	}

}
