#pragma once

#include "token_type.h"
#include "type.h"

namespace brisk {
	
	class TypeTable;

	const Type *type_from_token(TypeTable &type_table, TokenType token, bool ptr);

}