#include "type_exception.h"

#include <sstream>

#include "file.h"
#include "token_type.h"

namespace brisk {

	TypeException::TypeException(const std::vector<std::string> &errors)
	{
		std::ostringstream ss;
		
		for (auto& error : errors)
			ss << error << std::endl;

		message_ = ss.str();
	}
}
