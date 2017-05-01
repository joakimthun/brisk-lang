#pragma once

#include <sstream>

#include "typedef.h"
#include "brisk_exception.h"

namespace brisk {

	struct IOException : public BriskException
	{
		inline IOException(const std::string& message) : BriskException(message) {};
	};

	struct NotImplementedException : public BriskException
	{
		inline NotImplementedException() {};
	};
}
