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

	std::string StringView::to_unescaped_string() const
	{
		auto is_escaped = [](char c) { return c == '\\';};

		auto should_unescape_char = [](char c) {
			/*
				\t horizontal tab
				\v vertical tab
				\r carriage return
				\f ASCII Formfeed(FF)
			*/
			return	(c == 'n') || (c == 't') || (c == 'v') || (c == 'r') || (c == 'f');
		};

		auto unescape_char = [](char c) {
			switch (c)
			{
			case 'n': return '\n';
			case 't': return '\t';
			case 'v': return '\v';
			case 'r': return '\r';
			case 'f': return '\f';
			default: return c;
			}
		};

		std::string result;

		for (auto i = 0u; i < length_; i++)
		{
			auto has_next = (i + 1) < length_;
			if (has_next && is_escaped(data_[i]) && should_unescape_char(data_[i + 1]))
			{
				result += unescape_char(data_[i + 1]);
				i++;
			}
			else
			{
				result += data_[i];
			}
		}

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
