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
		auto raw_expr_ptr = expr.get();

		expr->start = parser.current_token();
		expr->end = parser.current_token();

		switch (parser.current_token().type)
		{
		case TokenType::IntLiteral: {
			if (parser.current_explicit_literal_type_expr() == nullptr)
			{
				// Default int literal type
				expr->type = parser.type_table().get("i64", false);
				expr->value.i64 = std::stoll(reinterpret_cast<const char*>(parser.current_token().raw_value.data()));
				parser.consume(TokenType::IntLiteral);
			}
			else
			{
				auto current_explicit_literal_type_expr = parser.current_explicit_literal_type_expr();
				auto current_token_raw_value = parser.current_token().raw_value.data();

				parser.defer([raw_expr_ptr, current_explicit_literal_type_expr, current_token_raw_value](BriskParser &parser) {
					auto type_id = current_explicit_literal_type_expr->type->id();
					switch (type_id)
					{
					case brisk::TypeID::U8:
						raw_expr_ptr->type = parser.type_table().get("u8", false);
						raw_expr_ptr->value.u8 = static_cast<u8>(std::stoul(reinterpret_cast<const char*>(current_token_raw_value)));
						break;
					case brisk::TypeID::I8:
						raw_expr_ptr->type = parser.type_table().get("i8", false);
						raw_expr_ptr->value.i8 = static_cast<i8>(std::stoi(reinterpret_cast<const char*>(current_token_raw_value)));
						break;
					case brisk::TypeID::U16:
						raw_expr_ptr->type = parser.type_table().get("u16", false);
						raw_expr_ptr->value.u16 = static_cast<u16>(std::stoul(reinterpret_cast<const char*>(current_token_raw_value)));
						break;
					case brisk::TypeID::I16:
						raw_expr_ptr->type = parser.type_table().get("i16", false);
						raw_expr_ptr->value.i16 = static_cast<i16>(std::stoi(reinterpret_cast<const char*>(current_token_raw_value)));
						break;
					case brisk::TypeID::U32:
						raw_expr_ptr->type = parser.type_table().get("u32", false);
						raw_expr_ptr->value.u32 = static_cast<u32>(std::stoul(reinterpret_cast<const char*>(current_token_raw_value)));
						break;
					case brisk::TypeID::I32:
						raw_expr_ptr->type = parser.type_table().get("i32", false);
						raw_expr_ptr->value.i32 = static_cast<i32>(std::stoi(reinterpret_cast<const char*>(current_token_raw_value)));
						break;
					case brisk::TypeID::U64:
						raw_expr_ptr->type = parser.type_table().get("u64", false);
						raw_expr_ptr->value.u64 = static_cast<u64>(std::stoull(reinterpret_cast<const char*>(current_token_raw_value)));
						break;
					case brisk::TypeID::I64:
					default:
						raw_expr_ptr->type = parser.type_table().get("i64", false);
						raw_expr_ptr->value.i64 = static_cast<i64>(std::stoll(reinterpret_cast<const char*>(current_token_raw_value)));
						break;
					}
				});

				parser.consume(TokenType::IntLiteral);
			}

			break;
		}
		case TokenType::StrLiteral: {
			expr->type = parser.type_table().get("u8", true);
			expr->str_value = parser.current_token().raw_value;
			expr->str_literal = true;
			parser.consume(TokenType::StrLiteral);
			break;
		}
		default:
			throw BriskException("LiteralParser::parse -> Unsupported type");
		}

		return expr;
	}

}