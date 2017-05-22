#pragma once

#include "parser.h"

namespace brisk {

	struct FnCallParser : public Parser
	{
		std::unique_ptr<Expr> parse(BriskParser &parser) override;
	};

}
