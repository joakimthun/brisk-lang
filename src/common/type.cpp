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

	const Expr *FnType::expr() const 
	{
		return &decl_expr;
	}

}
