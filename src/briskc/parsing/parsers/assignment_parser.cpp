#include "assignment_parser.h"

#include "../brisk_parser.h"
#include "ast/ast.h"
#include "../../exceptions/parsing_exception.h"

namespace brisk {

	std::unique_ptr<Expr> AssignmentParser::parse(BriskParser & parser, std::unique_ptr<Expr> left)
	{
		auto expr = std::make_unique<AssignExpr>();
		expr->start = parser.current_token();
		parser.consume(TokenType::Equals);
		expr->left = std::move(left);

		const auto identifier = dynamic_cast<IdentifierExpr*>(expr->left.get());
		if (identifier == nullptr)
		{
			// TODO: Add more accepted left-hand side expression types once we add them
			throw ParsingException("The left-hand side of the assignment expression is not assignable", expr->start);
		}

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
