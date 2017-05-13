#pragma once

#include "typedef.h"

namespace brisk {

	class ByteBuffer
	{
	public:
		ByteBuffer();
		~ByteBuffer();

		const void *data() const;
		u32 length() const;
		void write(u8 val);
		void write(const void *data, u32 length);
	private:
		void *data_;
		u32 length_;
		u32 offset_;
	};

}