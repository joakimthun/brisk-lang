#pragma once

#include "type.h"
#include "ast/ast.h"
#include "type_table.h"

namespace brisk {
	namespace types {

		bool try_static_convert(Expr &expr, const Type *target, TypeTable &type_table);

	}
}
