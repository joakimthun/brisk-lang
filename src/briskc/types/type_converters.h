#pragma once

#include "type.h"
#include "ast/ast.h"

namespace brisk {
	namespace types {

		void convert(Expr &expr, const Type *target);

	}
}
