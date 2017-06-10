#include "var_decl_parser.h"

#include "ast/ast.h"
#include "../brisk_parser.h"

namespace brisk {

	VarDeclParser::VarDeclParser(bool explicit_type)
		:
		explicit_type_(explicit_type)
	{
	}

	std::unique_ptr<Expr> VarDeclParser::parse(BriskParser &parser)
	{
		auto expr = std::make_unique<VarDeclExpr>();
		auto raw_expr_ptr = expr.get();

		expr->start = parser.current_token();

		if (explicit_type_)
		{
			parser.parse_type(raw_expr_ptr);
		}
		else
		{
			parser.consume(TokenType::Let);
		}

		const auto next = parser.peek();
		if (next.type == TokenType::Mut)
		{
			parser.consume(TokenType::Mut);
			expr->mut = true;
		}

		expr->name = parser.current_token().raw_value;
		parser.consume(TokenType::Identifier);
		parser.consume(TokenType::Equals);
		expr->expr = parser.parse_expr();

		expr->end = parser.current_token();

		if (!explicit_type_)
		{
			parser.defer([raw_expr_ptr](auto &parser) {
				raw_expr_ptr->type = raw_expr_ptr->expr->type;
			});
		}

		parser.current_scope()->add_var(expr.get());

		return expr;
	}

}
