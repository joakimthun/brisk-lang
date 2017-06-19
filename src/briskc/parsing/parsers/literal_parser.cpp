#include "literal_parser.h"

#include <string>

#include "../brisk_parser.h"
#include "ast/ast.h"
#include "type.h"

#include "exceptions.h"

namespace brisk {

	std::unique_ptr<Expr> LiteralParser::parse(BriskParser &parser)
	{
		auto expr = std::make_unique<LiteralExpr>();

		expr->start = parser.current_token();
		expr->end = parser.current_token();

		switch (parser.current_token().type)
		{
		case TokenType::IntLiteral: {
			expr->type = parser.type_table().get("i64", false);
			const i64 value = std::stoll(reinterpret_cast<const char*>(parser.current_token().raw_value.data()));
			expr->value.int64 = value;
			parser.consume(TokenType::IntLiteral);
			break;
		}
		case TokenType::UIntLiteral: {
			expr->type = parser.type_table().get("u64", false);
			const u64 value = std::stoull(reinterpret_cast<const char*>(parser.current_token().raw_value.data()));
			expr->value.uint64 = value;
			parser.consume(TokenType::UIntLiteral);
			break;
		}
		case TokenType::StrLiteral: {
			expr->type = parser.type_table().get("u8", true);
			expr->str_value = parser.current_token().raw_value;
			parser.consume(TokenType::StrLiteral);
			break;
		}
		default:
			throw BriskException("LiteralParser::parse -> Unsupported type");
		}

		return expr;
	}

}