#pragma once

#include "parser.h"

namespace brisk {

	struct RetParser : public Parser
	{
		std::unique_ptr<Expr> parse(BriskParser &parser) override;
	};

}