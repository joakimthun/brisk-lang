#include "type_converters.h"

#include "brisk_exception.h"

namespace brisk {
	namespace types {

		void convert(Expr &expr, const Type *target)
		{
			auto literal_expr = expr.as<LiteralExpr>();
			const auto primitive_type = target->as_const<PrimitiveType>();
			if (literal_expr != nullptr && (primitive_type != nullptr && primitive_type->is_integral()))
			{
				literal_expr->type = primitive_type;
				return;
			}

			// Should never happen
			throw BriskException("types::convert can not convert expression to type: " + target->name());
		}

	}

}
