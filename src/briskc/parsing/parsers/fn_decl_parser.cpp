#include "fn_decl_parser.h"

#include "../brisk_parser.h"
#include "ast/ast.h"
#include "../../types/type_util.h"

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

		while (parser.current_token().type != TokenType::RParen)
		{
			const auto arg_type = type_from_token(parser.type_table(), parser.current_token().type, false);
			parser.consume();
			const auto arg_name = parser.current_token().raw_value;
			parser.consume(TokenType::Identifier);

			expr->args.push_back(std::make_unique<FnArg>(arg_name, arg_type));

			if (parser.current_token().type == TokenType::Comma)
				parser.consume(TokenType::Comma);
		}

		parser.consume(TokenType::RParen);

		parser.consume(TokenType::RArrow);

		expr->return_type = type_from_token(parser.type_table(), parser.current_token().type, false);
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
