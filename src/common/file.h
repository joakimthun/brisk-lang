#pragma once

#include <string>
#include <memory>

#include "typedef.h"
#include "byte_buffer.h"

namespace brisk {

	struct File
	{
		std::unique_ptr<u8> content;
		std::string path;
		u64 length;
	};

	std::unique_ptr<File> open_file(const std::string &path);
	void write_file(const std::string &path, const ByteBuffer &data);
}