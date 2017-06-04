#include "type_util.h"

#include "brisk_exception.h"
#include "type_table.h"

namespace brisk {

	const Type *type_from_token(TypeTable &type_table, TokenType token, bool ptr)
	{
		switch (token)
		{
		case TokenType::U8:
			return type_table.get("u8", ptr);
		case TokenType::I8:
			return type_table.get("i8", ptr);
		case TokenType::U16:
			return type_table.get("u16", ptr);
		case TokenType::I16:
			return type_table.get("i16", ptr);
		case TokenType::U32:
			return type_table.get("u32", ptr);
		case TokenType::I32:
			return type_table.get("i32", ptr);
		case TokenType::U64:
			return type_table.get("u64", ptr);
		case TokenType::I64:
			return type_table.get("i64", ptr);
		case TokenType::Float:
			return type_table.get("float", ptr);
		case TokenType::Double:
			return type_table.get("double", ptr);
		case TokenType::Void:
			return type_table.get("void", ptr);
		default:
			throw BriskException("type_from_token: Unknown token type");
		}
	}

}