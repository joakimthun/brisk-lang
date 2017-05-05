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
		if (!peek_queue_.empty())
		{
			const auto next_token = peek_queue_.front();
			peek_queue_.erase(peek_queue_.begin());
			return next_token;
		}

		return next_internal();
	}

	const Token &Lexer::peek(u16 offset)
	{
		if (peek_queue_.size() > 0 && peek_queue_.back().type == TokenType::Eof)
			return peek_queue_.back();

		if (peek_queue_.size() >= offset)
			return peek_queue_[offset - 1];

		while (peek_queue_.size() < offset)
		{
			peek_queue_.push_back(next_internal());
			if (peek_queue_.back().type == TokenType::Eof)
				return peek_queue_.back();
		}

		return peek_queue_.back();
	}

	Token Lexer::next_internal()
	{
		start:
		if (current_.eof || (current_offset_ >= file_->length))
			return Token(TokenType::Eof, nullptr, 0, 0, 0, 0, nullptr);

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
		case '*': {
			consume();
			return create_token(TokenType::Star, start_offset);
		}
		case '/': {
			consume();
			return create_token(TokenType::Slash, start_offset);
		}
		case '\n': {
			consume_newline();
			goto start;
		}
		default:
			break;
		}

		if (is_digit())
			return read_digit(start_offset);


		throw UnrecognizedTokenException(current_.value, row_, column_, file_->path);
	}

	Token Lexer::create_token(TokenType type, u64 start_offset)
	{
		return create_token(type, start_offset, TokenValue{ 0 });
	}

	Token Lexer::create_token(TokenType type, u64 start_offset, TokenValue value)
	{
		const auto length = current_offset_ - start_offset;
		const auto column_start = column_ - length;

		return Token(type, 
			file_->content.get() + start_offset, 
			length,
			row_, 
			column_start,
			column_,
			value,
			file_.get());
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

	void Lexer::consume_newline()
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
