#include "grammar.h"

#include "precedence.h"
#include "parsers/identifier_parser.h"
#include "parsers/bin_expr_parser.h"
#include "parsers/literal_parser.h"
#include "parsers/assignment_parser.h"
#include "parsers/fn_decl_parser.h"
#include "parsers/ret_parser.h"

namespace brisk {

	typedef std::pair<TokenType, std::unique_ptr<InfixParser>> infix_pair;
	typedef std::pair<TokenType, std::unique_ptr<Parser>> expr_pair;
	typedef std::pair<TokenType, std::unique_ptr<Parser>> top_expr_pair;

	Grammar::Grammar()
	{
		init();
	}

	InfixParser *Grammar::get_infix_parser(TokenType type)
	{
		auto it = infix_parsers_.find(type);
		if (it != infix_parsers_.end())
		{
			return it->second.get();
		}

		return nullptr;
	}

	Parser *Grammar::get_expr_parser(TokenType type)
	{
		auto it = expr_parsers_.find(type);
		if (it != expr_parsers_.end())
		{
			return it->second.get();
		}

		return nullptr;
	}

	Parser *Grammar::get_top_expr_parser(TokenType type)
	{
		auto it = top_expr_parsers_.find(type);
		if (it != top_expr_parsers_.end())
		{
			return it->second.get();
		}

		return nullptr;
	}

	void Grammar::init()
	{
		// Top expr
		top_expr_parsers_.insert(top_expr_pair(TokenType::Fn, std::make_unique<FnDeclParser>()));

		// Infix
		infix_parsers_.insert(infix_pair(TokenType::Plus, std::make_unique<BinExprParser>(Precedence::Sum)));
		infix_parsers_.insert(infix_pair(TokenType::Minus, std::make_unique<BinExprParser>(Precedence::Sum)));
		infix_parsers_.insert(infix_pair(TokenType::Star, std::make_unique<BinExprParser>(Precedence::Product)));
		infix_parsers_.insert(infix_pair(TokenType::Slash, std::make_unique<BinExprParser>(Precedence::Product)));
		infix_parsers_.insert(infix_pair(TokenType::Equals, std::make_unique<AssignmentParser>()));

		// Expr
		expr_parsers_.insert(expr_pair(TokenType::I32Literal, std::make_unique<LiteralParser>()));
		expr_parsers_.insert(expr_pair(TokenType::Identifier, std::make_unique<IdentifierParser>()));
		expr_parsers_.insert(expr_pair(TokenType::Ret, std::make_unique<RetParser>()));
	}

}
