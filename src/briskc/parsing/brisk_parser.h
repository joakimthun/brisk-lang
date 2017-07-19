#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <stack>

#include "typedef.h"
#include "../lexing/lexer.h"
#include "ast/ast.h"
#include "grammar.h"
#include "ast/symbols.h"
#include "../types/type_table.h"

namespace brisk {

	struct ParseTypeResult
	{
		Token token;
		bool is_ptr = false;
	};

	class BriskParser
	{
	public:
		BriskParser(const std::string &filepath, TypeTable &type_table);

		std::unique_ptr<Ast> parse();
		const Token &current_token();
		void consume();
		void consume(TokenType type);
		const Token &peek(u16 offset = 1);
		std::unique_ptr<Expr> parse_top_expr();
		std::unique_ptr<Expr> parse_expr();
		std::unique_ptr<Expr> parse_expr(u8 precedence);
		void parse_type(Expr *expr);

		SymbolTable *current_scope();
		void push_scope(SymbolTable *scope);
		void pop_scope();

		void push_fn_context(const FnDeclExpr *fn);
		void pop_fn_context();
		const FnDeclExpr *current_fn_context();

		void push_explicit_literal_type(const Expr *type_expr);
		void pop_explicit_literal_type();
		const Expr *current_explicit_literal_type_expr();

		const Ast &current_pkg() const;
		TypeTable &type_table();

		void defer(std::function<void(BriskParser &parser)> fn);

	private:
		u8 get_precedence();
		void parse_pkg(Ast &ast);
		void push_pkg_context(const std::unique_ptr<Ast> &ast);
		void pop_pkg_context();

		Lexer lexer_;
		Token current_token_;
		Grammar grammar_;
		SymbolTable *current_scope_;
		std::stack<const Ast*> pkg_context_;
		TypeTable &type_table_;
		std::vector<std::function<void(BriskParser &parser)>> defered_calls_;
		std::stack<const FnDeclExpr*> fn_context_;
		std::stack<const Expr*> explicit_literal_types_;
	};
}

