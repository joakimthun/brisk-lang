#pragma once

#include <memory>
#include <array>
#include <unordered_map>
#include <functional>

#include "typedef.h"
#include "token_type.h"
#include "parsers/parser.h"
#include "parsers/infix_parser.h"

typedef std::pair<brisk::TokenType, brisk::TokenType> ll2_key;

namespace std
{
	template <>
	struct hash<ll2_key>
	{
		// http://stackoverflow.com/questions/2634690/good-hash-function-for-a-2d-index
		size_t operator()(ll2_key const &k) const noexcept
		{
			return ((51 + std::hash<int>()(static_cast<int>(k.first))) *
					 51 + std::hash<int>()(static_cast<int>(k.second)));
		}
	};
}

namespace brisk {
	
	constexpr auto MAX_NUM_TOKENS = static_cast<u8>(TokenType::Eof);

	class Grammar
	{
	public:
		Grammar();

		InfixParser *get_infix_parser(TokenType type);
		Parser *get_expr_parser(TokenType type);
		Parser *get_top_expr_parser(TokenType type);
		Parser *get_ll2_parser(TokenType t1, TokenType t2);
		Parser *get_top_ll2_parser(TokenType t1, TokenType t2);
	private:
		void init();

		std::array<std::unique_ptr<InfixParser>, MAX_NUM_TOKENS> infix_parsers_;
		std::array<std::unique_ptr<Parser>, MAX_NUM_TOKENS> expr_parsers_;
		std::array<std::unique_ptr<Parser>, MAX_NUM_TOKENS> top_expr_parsers_;
		std::unordered_map<ll2_key, std::unique_ptr<Parser>> ll2_parsers_;
		std::unordered_map<ll2_key, std::unique_ptr<Parser>> top_ll2_parsers_;
	};

}
