#pragma once

#include <memory>
#include <unordered_map>

#include "token_type.h"
#include "parsers/parser.h"
#include "parsers/infix_parser.h"

namespace brisk {
	
	class Grammar
	{
	public:
		Grammar();

		InfixParser *get_infix_parser(TokenType type);
		Parser *get_expr_parser(TokenType type);
	private:
		void init();

		std::unordered_map<TokenType, std::unique_ptr<InfixParser>> infix_parsers_;
		std::unordered_map<TokenType, std::unique_ptr<Parser>> expr_parsers_;
	};

}
