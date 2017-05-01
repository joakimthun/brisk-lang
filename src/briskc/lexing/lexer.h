#pragma once

#include <string>
#include <memory>

#include "typedef.h"
#include "token.h"
#include "file.h"
#include "utf8.h"

namespace brisk {

	class Lexer
	{
	public:
		Lexer(const std::string &filepath);

		Token next();
		//Token peek();

	private:
		Token create_token(TokenType type, u64 start_offset);
		Token create_token(TokenType type, u64 start_offset, TokenValue value);
		void consume_whitespace();
		void consume();
		void consume_new_line();

		bool is_whitespace();
		bool is_identifier();
		bool is_digit();

		Token read_digit(u64 start_offset);

		std::unique_ptr<File> file_;
		u64 current_offset_;
		utf8::CodePoint current_;
		u32 row_;
		u32 column_;
	};
}

