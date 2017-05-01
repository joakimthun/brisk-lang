#include "util.h"

#include "exceptions.h"

#ifdef _WIN32
#include "win32.h"
#endif

namespace brisk {

	std::unique_ptr<u8> get_full_path_name(const std::string &path)
	{
#ifdef _WIN32
		return win32::get_full_path_name(path);
#else
		throw NotImplementedException();
#endif
	}

}