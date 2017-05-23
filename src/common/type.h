#pragma once

#include "typedef.h"

namespace brisk {

	enum class TypeID : u16
	{
		U8,
		I8,
		U16,
		I16,
		U32,
		I32,
		U64,
		I64,
		Float,
		Double
	};

	struct Type
	{
		inline Type() {};
		inline Type(TypeID id) : id(id), ptr(false) {}
		inline Type(TypeID id, bool ptr) : id(id), ptr(ptr) {}

		TypeID id;
		bool ptr;
	};

}