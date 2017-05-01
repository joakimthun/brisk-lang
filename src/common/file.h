#pragma once

#include <string>
#include <memory>

#include "typedef.h"
#include "util.h"

namespace brisk {

	struct File
	{
		inline ~File()
		{
			SAFE_DELETE(content);
		}

		u8 *content;
		u64 length;
	};

	std::unique_ptr<File> open_file(const std::string &path);
}