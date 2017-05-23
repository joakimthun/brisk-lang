#include "string_view.h"

namespace brisk {

	StringView::StringView()
		:
		data_(nullptr),
		length_(0)
	{
	}

	StringView::StringView(u8 *data, u32 length)
		:
		data_(data),
		length_(length)
	{
	}

	std::string StringView::to_string() const
	{
		std::string result;

		for (auto i = 0u; i < length_; i++)
			result += data_[i];

		return result;
	}

	u32 StringView::length() const
	{
		return length_;
	}

	const u8 * StringView::data() const
	{
		return data_;
	}

}
