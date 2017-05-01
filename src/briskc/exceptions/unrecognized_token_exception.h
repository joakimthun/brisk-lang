#pragma once

#include <string>

#include "typedef.h"
#include "brisk_exception.h"

namespace brisk {

	struct UnrecognizedTokenException : public BriskException
	{
		UnrecognizedTokenException(u32 token, u32 row, u32 column, const std::string &filepath);
	};
}
