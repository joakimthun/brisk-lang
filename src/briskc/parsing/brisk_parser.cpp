#include "brisk_parser.h"

#include <utility>

#include "exceptions.h"
#include "../exceptions/parsing_exception.h"
#include "../types/type_util.h"

namespace brisk {

	BriskParser::BriskParser(const std::string &filepath, TypeTable &type_table)
		:
		lexer_(filepath),
		current_scope_(nullptr),
		type_table_(type_table)
	{
		consume();
	}

	std::unique_ptr<Ast> BriskParser::parse()
	{
		auto ast = std::make_unique<Ast>();
		ast->file = lexer_.file().path;
		push_scope(&ast->symbol_table);
		parse_pkg(*ast);

		while (current_token_.type != TokenType::Eof)
		{
			ast->exprs.push_back(parse_top_expr());
		}

		pop_scope();

		for (auto &call : defered_calls_)
			call(*this);

		return ast;
	}

	const Token &BriskParser::current_token()
	{
		return current_token_;
	}

	void BriskParser::consume()
	{
		current_token_ = lexer_.next();
	}

	void BriskParser::consume(TokenType type)
	{
		if (current_token_.type != type)
			throw ParsingException(type, current_token_);

		consume();
	}

	const Token &BriskParser::peek(u16 offset)
	{
		return lexer_.peek(offset);
	}

	std::unique_ptr<Expr> BriskParser::parse_top_expr()
	{
		auto parser = grammar_.get_top_ll2_parser(current_token_.type, lexer_.peek().type);

		if(parser == nullptr)
			parser = grammar_.get_top_expr_parser(current_token_.type);

		if(parser == nullptr)
			throw ParsingException(current_token_);

		return parser->parse(*this);
	}

	std::unique_ptr<Expr> BriskParser::parse_expr()
	{
		return parse_expr(0);
	}

	std::unique_ptr<Expr> BriskParser::parse_expr(u8 precedence)
	{
		auto expr_parser = grammar_.get_ll2_parser(current_token_.type, lexer_.peek().type);

		if (expr_parser == nullptr)
			expr_parser = grammar_.get_expr_parser(current_token_.type);

		if (expr_parser == nullptr)
			throw ParsingException(current_token_);

		auto left = expr_parser->parse(*this);

		while (precedence < get_precedence())
		{
			auto infix_parser = grammar_.get_infix_parser(current_token_.type);
			if (infix_parser == nullptr)
				throw ParsingException(current_token_);

			left = infix_parser->parse(*this, std::move(left));
		}

		return left;
	}

	void BriskParser::parse_type(Expr *expr)
	{
		ParseTypeResult result;
		result.token = current_token();
		consume();

		if (current_token().type == TokenType::Star)
		{
			result.is_ptr = true;
			consume();
		}

		defer([expr, result](auto &parser) {
			expr->type = type_from_token(parser.type_table(), result.token.type, result.is_ptr);
		});
	}

	SymbolTable *BriskParser::current_scope()
	{
		if (current_scope_ == nullptr)
			throw BriskException("Current scope is null!");

		return current_scope_;
	}

	void BriskParser::push_scope(SymbolTable *scope)
	{
		current_scope_ = scope;
	}

	void BriskParser::pop_scope()
	{
		if (current_scope_ == nullptr)
			throw BriskException("Current scope is null!");

		current_scope_ = current_scope_->parent();
	}

	void BriskParser::push_fn_context(const FnDeclExpr *fn)
	{
		fn_context_.push(fn);
	}

	void BriskParser::pop_fn_context()
	{
		fn_context_.pop();
	}

	const FnDeclExpr *BriskParser::current_fn_context()
	{
		return fn_context_.top();
	}

	void BriskParser::push_explicit_literal_type(const Expr *type_expr)
	{
		explicit_literal_types_.push(type_expr);
	}

	void BriskParser::pop_explicit_literal_type()
	{
		explicit_literal_types_.pop();
	}

	const Expr *BriskParser::current_explicit_literal_type_expr()
	{
		if (explicit_literal_types_.size() == 0)
			return nullptr;

		return explicit_literal_types_.top();
	}

	TypeTable &BriskParser::type_table()
	{
		return type_table_;
	}

	void BriskParser::defer(std::function<void(BriskParser &parser)> fn)
	{
		defered_calls_.push_back(fn);
	}

	u8 BriskParser::get_precedence()
	{
		auto infix_parser = grammar_.get_infix_parser(current_token_.type);
		if (infix_parser != nullptr)
			return infix_parser->precedence();

		return 0;
	}

	void BriskParser::parse_pkg(Ast & ast)
	{
		consume(TokenType::Pkg);
		ast.pkg_name = current_token_.raw_value;
		consume(TokenType::Identifier);
	}

}
