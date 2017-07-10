#include "byte_buffer.h"

#include <cstdlib>
#include <cstring>

#include "brisk_exception.h"
#include "util.h"

namespace brisk {

	ByteBuffer::ByteBuffer()
		:
		length_(0),
		data_(nullptr),
		offset_(0),
		capacity_(0)
	{
	}

	ByteBuffer::ByteBuffer(u32 initial_capacity)
		:
		length_(0),
		data_(nullptr),
		offset_(0),
		capacity_(initial_capacity)
	{
		data_ = std::calloc(initial_capacity, 1);
	}

	ByteBuffer::~ByteBuffer()
	{
		if (data_ != nullptr)
		{
			free(data_);
			data_ = nullptr;
		}
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
		resize_if_needed(length);
		std::memcpy(static_cast<u8*>(data_) + offset_, data, length);
		offset_ += length;
	}

	void ByteBuffer::write_at(u8 val, u32 offset)
	{
		write_at(&val, 1, offset);
	}

	void ByteBuffer::write_at(const void *data, u32 length, u32 offset)
	{
		if (offset_ == 0 || (offset + length) > offset_)
			throw BriskException("ByteBuffer::write_at: Can not write past current offset");

		std::memcpy(static_cast<u8*>(data_) + offset, data, length);
	}

	void ByteBuffer::resize_if_needed(u32 required_length)
	{
		if (data_ == nullptr)
		{
			data_ = std::calloc(required_length, 1);
			length_ = required_length;
			capacity_ = required_length;
		}
		else
		{
			length_ += required_length;

			if (length_ >= capacity_)
			{
				capacity_ += required_length;
				capacity_ *= 2;
				data_ = std::realloc(data_, capacity_);
			}
		}
	}

}