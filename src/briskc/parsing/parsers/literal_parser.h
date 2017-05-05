#pragma once

#include "parser.h"

namespace brisk {

	struct LiteralParser : public Parser
	{
		std::unique_ptr<Expr> parse(BriskParser &parser) override;
	};
	
}
