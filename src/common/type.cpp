#include "type.h"

#include "brisk_exception.h"

namespace brisk {

	Type type_from_token(TokenType token)
	{
		switch (token)
		{
		case TokenType::I32:
			return Type(TypeID::I32);
		default:
			throw BriskException("type_from_token: Unknown token type");
		}
	}

}
