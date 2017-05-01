#include "lexer.h"

#include <cctype>
#include <cstdlib>

#include "../exceptions/unrecognized_token_exception.h"

namespace brisk {

	Lexer::Lexer(const std::string &filepath)
		:
		current_offset_(0),
		current_(0, 0),
		row_(1),
		column_(1)
	{
		file_ = open_file(filepath);
		current_ = utf8::read_code_point(file_->content.get(), file_->length, current_offset_);
	}

	Token Lexer::next()
	{
		if (current_.eof || (current_offset_ >= file_->length))
			return Token(TokenType::Eof, nullptr, 0, 0, 0, 0);

		consume_whitespace();

		const auto start_offset = current_offset_;
		auto cval = (char)current_.value;

		switch (current_.value)
		{
		case '+': {
			consume();
			return create_token(TokenType::Plus, start_offset);
		}
		case '-': {
			consume();
			return create_token(TokenType::Minus, start_offset);
		}
		default:
			break;
		}

		if (is_digit())
			return read_digit(start_offset);


		throw UnrecognizedTokenException(current_.value, row_, column_, file_->path);
	}

	//Token Lexer::peek()
	//{
	//	return Token()
	//}

	Token Lexer::create_token(TokenType type, u64 start_offset)
	{
		return create_token(type, start_offset, TokenValue{ 0 });
	}

	Token Lexer::create_token(TokenType type, u64 start_offset, TokenValue value)
	{
		const auto length = current_offset_ - start_offset;
		auto column_start = column_ - length;

		return Token(type, 
			file_->content.get() + start_offset, 
			length,
			row_, 
			column_start,
			column_,
			value);
	}

	void Lexer::consume_whitespace()
	{
		while (is_whitespace())
			consume();
	}

	void Lexer::consume()
	{
		current_offset_ += current_.num_bytes;
		current_ = utf8::read_code_point(file_->content.get(), file_->length, current_offset_);
		column_++;
	}

	void Lexer::consume_new_line()
	{
		consume();
		row_++;
		column_ = 1;
	}

	bool Lexer::is_whitespace()
	{
		/* 
			\t horizontal tab
			\v vertical tab
			\r carriage return
			\f ASCII Formfeed(FF)
		*/

		return	(current_.value == ' ') || (current_.value == '\t') || (current_.value == '\v') || 
				(current_.value == '\r') || (current_.value == '\f');
	}

	bool Lexer::is_identifier()
	{
		return current_.value == '_' || std::isalpha(current_.value);
	}

	bool Lexer::is_digit()
	{
		return std::isdigit(current_.value);
	}

	Token Lexer::read_digit(u64 start_offset)
	{
		consume();

		while (is_digit())
		{
			consume();
		}

		const auto raw_value = file_->content.get() + start_offset;
		TokenValue value = { 0 };
		value.i32 = std::atoi(reinterpret_cast<char*>(raw_value));

		return create_token(TokenType::I32Literal, start_offset, value);
	}
}
