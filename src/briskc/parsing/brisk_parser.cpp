#include "brisk_parser.h"

#include <utility>

#include "exceptions.h"
#include "../exceptions/parsing_exception.h"

namespace brisk {

	BriskParser::BriskParser(const std::string &filepath)
		:
		lexer_(filepath)
	{
		consume();
	}

	std::unique_ptr<Ast> BriskParser::parse()
	{
		auto ast = std::make_unique<Ast>();

		while (current_token_.type != TokenType::Eof)
		{
			ast->stmts.push_back(parse_stmt());
		}

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

	std::unique_ptr<Expr> BriskParser::parse_expr()
	{
		return parse_expr(0);
	}

	std::unique_ptr<Expr> BriskParser::parse_expr(u8 precedence)
	{
		auto expr_parser = grammar_.get_expr_parser(current_token_.type);

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

	std::unique_ptr<Stmt> BriskParser::parse_stmt()
	{
		return std::unique_ptr<Stmt>();
	}

	u8 BriskParser::get_precedence()
	{
		auto infix_parser = grammar_.get_infix_parser(current_token_.type);
		if (infix_parser != nullptr)
			return infix_parser->precedence();

		return 0;
	}

}
