#include "type_converters.h"

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>


#include "brisk_exception.h"

namespace brisk {
	namespace types {

		bool can_convert_integral_literal(Expr &expr, const Type *target)
		{
			if (expr.node_type() != NodeType::Literal)
				return false;

			const auto target_as_primitive = target->as_const<PrimitiveType>();
			const auto source_as_primitive = expr.type->as_const<PrimitiveType>();
			if(target_as_primitive == nullptr || source_as_primitive == nullptr)
				return false;

			return target_as_primitive->is_integral() && source_as_primitive->is_integral();
		}

		void convert_integral_literal(Expr &expr, const Type *target, TypeTable &type_table)
		{
			auto literal_expr = expr.as<LiteralExpr>();

			switch (target->id())
			{
			case TypeID::U8:
				literal_expr->type = type_table.get("u8", false);
				break;
			case TypeID::I8:
				literal_expr->type = type_table.get("i8", false);
				break;
			case TypeID::U16:
				literal_expr->type = type_table.get("u16", false);
				break;
			case TypeID::I16:
				literal_expr->type = type_table.get("i16", false);
				break;
			case TypeID::U32:
				literal_expr->type = type_table.get("u32", false);
				break;
			case TypeID::I32:
				literal_expr->type = type_table.get("i32", false);
				break;
			case TypeID::U64:
				literal_expr->type = type_table.get("u64", false);
				break;
			case TypeID::I64:
				literal_expr->type = type_table.get("i64", false);
				break;
			default:
				// Should never happen
				throw BriskException("types::convert_integral_literal can not convert to type: " + target->name());
			}
		}

		typedef std::pair<std::function<bool(Expr &expr, const Type *target)>, std::function<void(Expr &expr, const Type *target, TypeTable &type_table)>> static_type_converter;
		static auto static_type_conversions = std::unordered_map<std::string, std::vector<static_type_converter>>
		{
			{ 
				"u8", 
				{
					static_type_converter(can_convert_integral_literal, convert_integral_literal)
				}
			},
			{ 
				"i8", 
				{
					static_type_converter(can_convert_integral_literal, convert_integral_literal)
				}
			},
			{ 
				"u16",
				{
					static_type_converter(can_convert_integral_literal, convert_integral_literal)
				}
			},
			{ 
				"i16",
				{
					static_type_converter(can_convert_integral_literal, convert_integral_literal)
				}
			},
			{ 
				"u32",
				{
					static_type_converter(can_convert_integral_literal, convert_integral_literal)
				}
			},
			{ 
				"i32",
				{
					static_type_converter(can_convert_integral_literal, convert_integral_literal)
				}
			},
			{ 
				"u64",
				{
					static_type_converter(can_convert_integral_literal, convert_integral_literal)
				}
			},
			{ 
				"i64",
				{
					static_type_converter(can_convert_integral_literal, convert_integral_literal)
				}
			}
		};

		bool try_static_convert(Expr &expr, const Type *target, TypeTable &type_table)
		{
			auto& it = static_type_conversions.find(target->name());
			if (it == static_type_conversions.end())
			{
				return false;
			}

			const auto& converters = it->second;
			for (const auto& converter : converters)
			{
				const auto can_convert = converter.first;
				const auto converter_fn = converter.second;
				if (can_convert(expr, target))
				{
					converter_fn(expr, target, type_table);
					return true;
				}
			}

			return false;
		}
	}

}
