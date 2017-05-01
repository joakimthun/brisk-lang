#pragma once

#include <string>
#include <memory>

#include "typedef.h"
#include "util.h"

namespace brisk {

	struct File
	{
		std::unique_ptr<u8> content;
		std::string path;
		u64 length;
	};

	std::unique_ptr<File> open_file(const std::string &path);
}