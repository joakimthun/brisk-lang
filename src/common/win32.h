#pragma once

#include <memory>
#include <string>

#include "typedef.h"

#ifdef _WIN32

namespace brisk {
	namespace win32 {

		std::unique_ptr<u8> get_full_path_name(const std::string &path);

	}
}

#endif