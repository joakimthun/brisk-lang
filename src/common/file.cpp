#include "file.h"

#include <stdlib.h>
#include <fstream>

#include "exceptions/brisk_io_exception.h"

namespace brisk {

	std::unique_ptr<File> open_file(const std::string &path)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);

		if (!file.good())
			throw BriskIOException("Could not open '" + path + "'");

		auto result = std::make_unique<File>();
		result->length = file.tellg();
		file.seekg(0, std::ios::beg);

		result->content = static_cast<u8*>(std::calloc(result->length, 1));

		auto &read_result = file.read((char*)result->content, result->length);
		
		file.close();

		if(!read_result)
			throw BriskIOException("Could not read '" + path + "'");

		return result;
		
	}

}
