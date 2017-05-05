#pragma once

#include <string>

#include "typedef.h"

namespace brisk {

	class StringView
	{
	public:
		StringView();
		StringView(u8 *data, u32 length);

		std::string to_string() const;
		u32 length() const;
	private:
		u8 *data_;
		u32 length_;
	};

}
