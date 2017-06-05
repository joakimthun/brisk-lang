#include "identifier_parser.h"

#include "ast/ast.h"
#include "../brisk_parser.h"

namespace brisk {

	std::unique_ptr<Expr> IdentifierParser::parse(BriskParser &parser)
	{
		auto expr = std::make_unique<IdentifierExpr>();

		expr->start = parser.current_token();
		expr->end = parser.current_token();

		expr->name = parser.current_token().raw_value;
		parser.consume(TokenType::Identifier);

		auto raw_expr_ptr = expr.get();
		parser.defer([raw_expr_ptr](BriskParser &parser) {
			auto symbol = parser.current_scope()->find<VarSymbol>(raw_expr_ptr->name.to_string());
			raw_expr_ptr->type = parser.type_table().get(raw_expr_ptr->name, false);
		});

		return expr;
	}

}
