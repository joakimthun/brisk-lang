#pragma once

#include <string>
#include <memory>

#include "typedef.h"
#include "../lexing/lexer.h"
#include "ast/ast.h"
#include "grammar.h"
#include "ast/symbols.h"
#include "../types/type_table.h"

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
		std::unique_ptr<Expr> parse_top_expr();
		std::unique_ptr<Expr> parse_expr();
		std::unique_ptr<Expr> parse_expr(u8 precedence);

		SymbolTable *current_scope();
		void push_scope(SymbolTable *scope);
		void pop_scope();

		TypeTable &type_table();

	private:
		u8 get_precedence();
		void parse_pkg(Ast &ast);

		Lexer lexer_;
		Token current_token_;
		Grammar grammar_;
		SymbolTable *current_scope_;
		TypeTable type_table_;
	};
}

