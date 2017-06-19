#include "type.h"

#include "ast/ast.h"

namespace brisk {

	bool Type::equals(const Type *other) const
	{
		if(is_ptr() != other->is_ptr())
			return false;

		if(id() != other->id())
			return false;

		// TODO: Handle user defined types

		return true;
	}

	bool Type::can_convert_to(const Type *target) const
	{
		const auto this_primitive = this->as_const<PrimitiveType>();
		const auto target_primitive = target->as_const<PrimitiveType>();
		if (this_primitive != nullptr && target_primitive != nullptr)
		{
			if (this_primitive->is_integral() && target_primitive->is_integral())
			{
				return true;
			}
		}

		return false;
	}

	const Expr *FnType::expr() const 
	{
		return &decl_expr;
	}

}
