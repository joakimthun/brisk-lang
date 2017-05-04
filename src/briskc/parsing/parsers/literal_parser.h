#pragma once

#include "parser.h"

namespace brisk {

	class LiteralParser : public Parser
	{
	public:
		std::unique_ptr<Expr> parse(BriskParser &parser) override;

	};
	
}
