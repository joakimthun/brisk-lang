#pragma once

#include <string>
#include <vector>

#include "typedef.h"
#include "brisk_exception.h"

namespace brisk {

	struct TypeException : public BriskException
	{
		TypeException(const std::vector<std::string> &errors);
	};
}
