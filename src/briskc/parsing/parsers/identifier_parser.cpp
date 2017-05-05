#include "identifier_parser.h"

#include "ast/ast.h"
#include "../brisk_parser.h"

namespace brisk {

	std::unique_ptr<Expr> IdentifierParser::parse(BriskParser & parser)
	{
		auto expr = std::make_unique<IdentifierExpr>();

		expr->start = parser.current_token();
		expr->end = parser.current_token();

		expr->name = parser.current_token().raw_value;
		parser.consume(TokenType::Identifier);

		return expr;
	}

}
