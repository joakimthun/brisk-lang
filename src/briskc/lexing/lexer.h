#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "typedef.h"
#include "token.h"
#include "file.h"
#include "utf8.h"

namespace brisk {

	struct KeywordMatch
	{
		inline KeywordMatch() : ttype(TokenType::Eof), length(0), match(false) {}
		TokenType ttype;
		u32 length;
		bool match;
	};

	class Lexer
	{
	public:
		Lexer(const std::string &filepath);

		Token next();
		const Token &peek(u16 offset = 1);

	private:
		Token next_internal();
		Token create_token(TokenType type, u64 start_offset);
		Token create_token(TokenType type, u64 start_offset, TokenValue value);
		void consume_whitespace();
		void consume();
		bool consume(char c);
		void consume_newline();

		bool is_whitespace();
		bool is_identifier();
		KeywordMatch is_keyword();
		bool is_digit();

		Token read_digit(u64 start_offset);
		Token read_identifier(u64 start_offset);
		Token read_keyword(u64 start_offset, u32 length, TokenType type);
		Token read_string(u64 start_offset);

		std::unique_ptr<File> file_;
		u64 current_offset_;
		utf8::CodePoint current_;
		u32 row_;
		u32 column_;
		std::vector<Token> peek_queue_;
		std::unordered_map<std::string, TokenType> keywords_;
	};
}

