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
#include "parsers/if_parser.h"

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

	Parser *Grammar::get_top_ll2_parser(TokenType t1, TokenType t2)
	{
		const auto key = ll2_key(t1, t2);
		auto it = top_ll2_parsers_.find(key);
		if (it != top_ll2_parsers_.end())
		{
			return it->second.get();
		}

		return nullptr;
	}

	void Grammar::init()
	{
		// Top LL2 parsers
		top_ll2_parsers_.insert(ll2_pair(ll2_key(TokenType::Ext, TokenType::Fn), std::make_unique<FnDeclParser>(true)));

		// LL2 parsers
		ll2_parsers_.insert(ll2_pair(ll2_key(TokenType::Identifier, TokenType::LParen), std::make_unique<FnCallParser>()));

		// Top expr
		top_expr_parsers_[arr_index(TokenType::Fn)] = std::make_unique<FnDeclParser>();

		// Infix
		infix_parsers_[arr_index(TokenType::Plus)] = std::make_unique<BinExprParser>(Precedence::Sum);
		infix_parsers_[arr_index(TokenType::Minus)] = std::make_unique<BinExprParser>(Precedence::Sum);
		infix_parsers_[arr_index(TokenType::Star)] = std::make_unique<BinExprParser>(Precedence::Product);
		infix_parsers_[arr_index(TokenType::Slash)] = std::make_unique<BinExprParser>(Precedence::Product);
		infix_parsers_[arr_index(TokenType::Modulus)] = std::make_unique<BinExprParser>(Precedence::Product);
		infix_parsers_[arr_index(TokenType::LogOr)] = std::make_unique<BinExprParser>(Precedence::LogOr);
		infix_parsers_[arr_index(TokenType::LogAnd)] = std::make_unique<BinExprParser>(Precedence::LogAnd);
		infix_parsers_[arr_index(TokenType::BitOr)] = std::make_unique<BinExprParser>(Precedence::BitOr);
		infix_parsers_[arr_index(TokenType::BitAnd)] = std::make_unique<BinExprParser>(Precedence::BitAnd);
		infix_parsers_[arr_index(TokenType::DoubleEquals)] = std::make_unique<BinExprParser>(Precedence::Equality);
		infix_parsers_[arr_index(TokenType::NotEqual)] = std::make_unique<BinExprParser>(Precedence::Equality);
		infix_parsers_[arr_index(TokenType::Equals)] = std::make_unique<AssignmentParser>();

		// Expr
		expr_parsers_[arr_index(TokenType::IntLiteral)] = std::make_unique<LiteralParser>();
		expr_parsers_[arr_index(TokenType::StrLiteral)] = std::make_unique<LiteralParser>();
		expr_parsers_[arr_index(TokenType::TrueLiteral)] = std::make_unique<LiteralParser>();
		expr_parsers_[arr_index(TokenType::FalseLiteral)] = std::make_unique<LiteralParser>();
		expr_parsers_[arr_index(TokenType::Identifier)] = std::make_unique<IdentifierParser>();
		expr_parsers_[arr_index(TokenType::Ret)] = std::make_unique<RetParser>();
		expr_parsers_[arr_index(TokenType::If)] = std::make_unique<IfParser>();

		expr_parsers_[arr_index(TokenType::Let)] = std::make_unique<VarDeclParser>(false);
		expr_parsers_[arr_index(TokenType::U8)] = std::make_unique<VarDeclParser>(true);
		expr_parsers_[arr_index(TokenType::I8)] = std::make_unique<VarDeclParser>(true);
		expr_parsers_[arr_index(TokenType::U16)] = std::make_unique<VarDeclParser>(true);
		expr_parsers_[arr_index(TokenType::I16)] = std::make_unique<VarDeclParser>(true);
		expr_parsers_[arr_index(TokenType::U32)] = std::make_unique<VarDeclParser>(true);
		expr_parsers_[arr_index(TokenType::I32)] = std::make_unique<VarDeclParser>(true);
		expr_parsers_[arr_index(TokenType::U64)] = std::make_unique<VarDeclParser>(true);
		expr_parsers_[arr_index(TokenType::I64)] = std::make_unique<VarDeclParser>(true);
		expr_parsers_[arr_index(TokenType::Float)] = std::make_unique<VarDeclParser>(true);
		expr_parsers_[arr_index(TokenType::Double)] = std::make_unique<VarDeclParser>(true);
		expr_parsers_[arr_index(TokenType::Bool)] = std::make_unique<VarDeclParser>(true);
	}

}
