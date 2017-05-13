#include "file.h"

#include <stdlib.h>
#include <fstream>

#include "exceptions.h"
#include "util.h"

namespace brisk {

	std::unique_ptr<File> open_file(const std::string &path)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);

		if (!file.good())
			throw IOException("Could not open '" + path + "'");

		auto result = std::make_unique<File>();
		result->length = file.tellg();
		file.seekg(0, std::ios::beg);

		const auto full_path_name = get_full_path_name(path.c_str());
		result->path = std::string(reinterpret_cast<char*>(full_path_name.get()));

		result->content = std::unique_ptr<u8>(static_cast<u8*>(std::calloc(result->length, 1)));
		auto &read_result = file.read(reinterpret_cast<char*>(result->content.get()), result->length);
		
		file.close();

		if(!read_result)
			throw IOException("Could not read '" + path + "'");

		return result;
	}

	void write_file(const std::string & path, const ByteBuffer & data)
	{
		std::ofstream file(path, std::ios::binary);
		file.write(reinterpret_cast<const char*>(data.data()), data.length());
		file.close();
	}
}
