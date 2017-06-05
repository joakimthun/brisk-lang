#include "fn_call_parser.h"

#include "ast/ast.h"
#include "../brisk_parser.h"
#include "../../exceptions/parsing_exception.h"

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

		auto raw_expr_ptr = expr.get();

		parser.defer([raw_expr_ptr](auto &parser) {
			auto callee = raw_expr_ptr->callee = parser.type_table().get<FnType>(raw_expr_ptr->name, false);
			if(callee == nullptr)
				throw ParsingException("No function with the name '" + raw_expr_ptr->name.to_string() + "' has been defined");

			raw_expr_ptr->callee = callee;
		});
		
		parser.defer([raw_expr_ptr](auto &parser) {
			auto callee = raw_expr_ptr->callee;
			raw_expr_ptr->type = callee->decl_expr.return_type;
		});

		return expr;
	}

}
