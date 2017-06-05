#include "fn_decl_parser.h"

#include "../brisk_parser.h"
#include "ast/ast.h"
#include "../../types/type_util.h"

namespace brisk {

	FnDeclParser::FnDeclParser()
		:
		ext_(false)
	{
	}

	FnDeclParser::FnDeclParser(bool ext)
		:
		ext_(ext)
	{
	}

	std::unique_ptr<Expr> FnDeclParser::parse(BriskParser &parser)
	{
		if (ext_)
			parser.consume(TokenType::Ext);

		parser.consume(TokenType::Fn);

		auto expr = std::make_unique<FnDeclExpr>(parser.current_scope(), ext_);
		parser.push_scope(&expr->symbol_table);

		expr->start = parser.current_token();

		expr->name = parser.current_token().raw_value;
		parser.consume(TokenType::Identifier);

		parser.consume(TokenType::LParen);

		while (parser.current_token().type != TokenType::RParen)
		{
			auto arg_expr = std::make_unique<FnArgExpr>();
			arg_expr->start = parser.current_token();

			arg_expr->type = type_from_token(parser.type_table(), parser.current_token().type, false);
			parser.consume();
			arg_expr->name = parser.current_token().raw_value;
			parser.consume(TokenType::Identifier);

			arg_expr->end = parser.current_token();
			expr->args.push_back(std::move(arg_expr));

			if (parser.current_token().type == TokenType::Comma)
				parser.consume(TokenType::Comma);
		}

		parser.consume(TokenType::RParen);

		parser.consume(TokenType::RArrow);

		expr->return_type = type_from_token(parser.type_table(), parser.current_token().type, false);
		parser.consume();

		parser.type_table().add(expr->name, std::make_unique<FnType>(expr->name, *expr));

		if (ext_)
			return expr;

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
