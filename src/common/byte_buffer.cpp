#include "byte_buffer.h"

#include <cstdlib>
#include <cstring>

#include "util.h"

namespace brisk {

	ByteBuffer::ByteBuffer()
		:
		length_(0),
		data_(nullptr),
		offset_(0)
	{
	}

	ByteBuffer::~ByteBuffer()
	{
		if (data_ != nullptr)
			free(data_);

		data_ = nullptr;
	}

	const void *ByteBuffer::data() const
	{
		return data_;
	}

	u32 ByteBuffer::length() const
	{
		return length_;
	}

	void ByteBuffer::write(u8 val)
	{
		write(&val, 1);
	}

	void ByteBuffer::write(const void *data, u32 length)
	{
		if (data_ == nullptr)
		{
			data_ = std::calloc(length, 1);
			length_ = length;
		}
		else
		{ 
			length_ += length;
			data_ = std::realloc(data_, length_);
		}

		std::memcpy(static_cast<u8*>(data_) + offset_, data, length);
		offset_ += length;
	}

}