#pragma once

#include "brisk_exception.h"

namespace brisk {

	struct BriskIOException : public BriskException
	{
		inline BriskIOException(const std::string& message) : BriskException(message) {};
	};
}
