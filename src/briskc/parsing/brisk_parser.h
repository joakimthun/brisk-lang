#pragma once

#include <string>
#include <memory>

#include "typedef.h"
#include "../lexing/lexer.h"
#include "ast/ast.h"
#include "grammar.h"

namespace brisk {

	class BriskParser
	{
	public:
		BriskParser(const std::string &filepath);

		std::unique_ptr<Ast> parse();
		const Token &current_token();
		void consume();
		void consume(TokenType type);
		const Token &peek(u16 offset = 1);
		std::unique_ptr<Expr> parse_expr();
		std::unique_ptr<Expr> parse_expr(u8 precedence);

	private:
		std::unique_ptr<Stmt> parse_stmt();
		u8 get_precedence();

		Lexer lexer_;
		Token current_token_;
		Grammar grammar_;
	};
}

