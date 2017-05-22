#include "grammar.h"

#include "precedence.h"
#include "parsers/identifier_parser.h"
#include "parsers/bin_expr_parser.h"
#include "parsers/literal_parser.h"
#include "parsers/assignment_parser.h"
#include "parsers/fn_decl_parser.h"
#include "parsers/ret_parser.h"
#include "parsers/var_decl_parser.h"
#include "parsers/fn_call_parser.h"

namespace brisk {

	typedef std::pair<ll2_key, std::unique_ptr<Parser>> ll2_pair;
	constexpr auto arr_index(TokenType type)
	{
		return static_cast<u8>(type);
	}

	Grammar::Grammar()
	{
		init();
	}

	InfixParser *Grammar::get_infix_parser(TokenType type)
	{
		return infix_parsers_[arr_index(type)].get();
	}

	Parser *Grammar::get_expr_parser(TokenType type)
	{
		return expr_parsers_[arr_index(type)].get();
	}

	Parser *Grammar::get_top_expr_parser(TokenType type)
	{
		return top_expr_parsers_[arr_index(type)].get();
	}

	Parser *Grammar::get_ll2_parser(TokenType t1, TokenType t2)
	{
		const auto key = ll2_key(t1, t2);
		auto it = ll2_parsers_.find(key);
		if (it != ll2_parsers_.end())
		{
			return it->second.get();
		}

		return nullptr;
	}

	void Grammar::init()
	{
		// LL2 parsers
		ll2_parsers_.insert(ll2_pair(ll2_key(TokenType::Identifier, TokenType::LParen), std::make_unique<FnCallParser>()));

		// Top expr
		top_expr_parsers_[arr_index(TokenType::Fn)] = std::make_unique<FnDeclParser>();

		// Infix
		infix_parsers_[arr_index(TokenType::Plus)] = std::make_unique<BinExprParser>(Precedence::Sum);
		infix_parsers_[arr_index(TokenType::Minus)] = std::make_unique<BinExprParser>(Precedence::Sum);
		infix_parsers_[arr_index(TokenType::Star)] = std::make_unique<BinExprParser>(Precedence::Product);
		infix_parsers_[arr_index(TokenType::Slash)] = std::make_unique<BinExprParser>(Precedence::Product);
		infix_parsers_[arr_index(TokenType::Equals)] = std::make_unique<AssignmentParser>();

		// Expr
		expr_parsers_[arr_index(TokenType::I32Literal)] = std::make_unique<LiteralParser>();
		expr_parsers_[arr_index(TokenType::Identifier)] = std::make_unique<IdentifierParser>();
		expr_parsers_[arr_index(TokenType::Ret)] = std::make_unique<RetParser>();
		expr_parsers_[arr_index(TokenType::Let)] = std::make_unique<VarDeclParser>();
	}

}
