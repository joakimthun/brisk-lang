#include "lexer.h"

#include <cctype>
#include <cstdlib>

#include "../exceptions/unrecognized_token_exception.h"

namespace brisk {

	typedef std::pair<std::string, TokenType> kwe;

	Lexer::Lexer(const std::string &filepath)
		:
		current_offset_(0),
		current_(0, 0),
		row_(1),
		column_(1)
	{
		file_ = open_file(filepath);
		current_ = utf8::read_code_point(file_->content.get(), file_->length, current_offset_);

		// Keywords
		keywords_.insert(kwe("fn", TokenType::Fn));
		keywords_.insert(kwe("ret", TokenType::Ret));
		keywords_.insert(kwe("let", TokenType::Let));
		keywords_.insert(kwe("mut", TokenType::Mut));
		keywords_.insert(kwe("pkg", TokenType::Pkg));
		keywords_.insert(kwe("ext", TokenType::Ext));
		keywords_.insert(kwe("if", TokenType::If));

		keywords_.insert(kwe("u8", TokenType::U8));
		keywords_.insert(kwe("i8", TokenType::I8));
		keywords_.insert(kwe("u16", TokenType::U16));
		keywords_.insert(kwe("i16", TokenType::I16));
		keywords_.insert(kwe("u32", TokenType::U32));
		keywords_.insert(kwe("i32", TokenType::I32));
		keywords_.insert(kwe("u64", TokenType::U64));
		keywords_.insert(kwe("i64", TokenType::I64));
		keywords_.insert(kwe("float", TokenType::Float));
		keywords_.insert(kwe("double", TokenType::Double));
		keywords_.insert(kwe("void", TokenType::Void));

		keywords_.insert(kwe("bool", TokenType::Bool));
		keywords_.insert(kwe("false", TokenType::FalseLiteral));
		keywords_.insert(kwe("true", TokenType::TrueLiteral));
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

	const File &Lexer::file()
	{
		return *file_;
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

			if(consume('>'))
				return create_token(TokenType::RArrow, start_offset);

			return create_token(TokenType::Minus, start_offset);
		}
		case '*': {
			consume();
			return create_token(TokenType::Star, start_offset);
		}
		case '/': {
			consume();

			if (consume('/'))
			{
				while (current_.value != '\n')
				{
					if (current_.eof || (current_offset_ >= file_->length))
						break;

					consume();
				}

				goto start;
			}

			return create_token(TokenType::Slash, start_offset);
		}
		case '%': {
			consume();
			return create_token(TokenType::Modulus, start_offset);
		}
		case '=': {
			consume();

			if (consume('='))
				return create_token(TokenType::DoubleEquals, start_offset);

			return create_token(TokenType::Equals, start_offset);
		}
		case '!': {
			consume();

			if (consume('='))
				return create_token(TokenType::NotEqual, start_offset);

			return create_token(TokenType::Not, start_offset);
		}
		case '{': {
			consume();
			return create_token(TokenType::LBracket, start_offset);
		}
		case '}': {
			consume();
			return create_token(TokenType::RBracket, start_offset);
		}
		case '(': {
			consume();
			return create_token(TokenType::LParen, start_offset);
		}
		case ')': {
			consume();
			return create_token(TokenType::RParen, start_offset);
		}
		case ',': {
			consume();
			return create_token(TokenType::Comma, start_offset);
		}
		case '"': {
			return read_string(start_offset);
		}
		case '\n': {
			consume_newline();
			goto start;
		}
		case '|': {
			consume();

			if (consume('|'))
				return create_token(TokenType::LogOr, start_offset);

			return create_token(TokenType::BitOr, start_offset);
		}
		case '&': {
			consume();

			if (consume('&'))
				return create_token(TokenType::LogAnd, start_offset);

			return create_token(TokenType::BitAnd, start_offset);
		}
		default:
			break;
		}

		const auto keyword = is_keyword();
		if (keyword.match)
			return read_keyword(start_offset, keyword.length, keyword.ttype);

		if (is_identifier())
			return read_identifier(start_offset);

		if (is_digit())
			return read_digit(start_offset);


		throw UnrecognizedTokenException(current_.value, row_, column_, file_->path);
	}

	Token Lexer::create_token(TokenType type, u64 start_offset)
	{
		const auto length = current_offset_ - start_offset;
		const auto column_start = column_ - length;

		return Token(type, 
			file_->content.get() + start_offset, 
			length,
			row_, 
			column_start,
			column_,
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

	bool Lexer::consume(char c)
	{
		if (current_.value == c)
		{
			consume();
			return true;
		}

		return false;
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

	KeywordMatch Lexer::is_keyword()
	{
		u32 l = 0;
		std::string kw;
		auto c = utf8::read_code_point(file_->content.get(), file_->length, current_offset_ + l);

		while (std::isalpha(c.value) || std::isdigit(c.value))
		{
			// Fix this if we add unicode stuff in keywords
			kw += (char)c.value;
			l += c.num_bytes;
			c = utf8::read_code_point(file_->content.get(), file_->length, current_offset_ + l);
		}

		KeywordMatch m;
		if (kw.size() == 0)
			return m;

		auto it = keywords_.find(kw);
		if (it != keywords_.end())
		{
			m.length = l;
			m.ttype = it->second;
			m.match = true;
			return m;
		}

		return m;
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

		return create_token(TokenType::IntLiteral, start_offset);
	}

	Token Lexer::read_identifier(u64 start_offset)
	{
		consume();

		while (is_identifier() || is_digit())
		{
			consume();
		}

		return create_token(TokenType::Identifier, start_offset);
	}

	Token Lexer::read_keyword(u64 start_offset, u32 length, TokenType type)
	{
		for (auto i = 0u; i < length; i++)
			consume();

		return create_token(type, start_offset);
	}

	Token Lexer::read_string(u64 start_offset)
	{
		consume('"');
		bool escaped = false;

		while (current_.value != '"')
		{
			consume();
		}

		auto token = create_token(TokenType::StrLiteral, start_offset + 1);
		consume('"');

		return token;
	}
}
