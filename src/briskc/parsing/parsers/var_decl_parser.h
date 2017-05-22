#pragma once

#include <memory>

#include "parser.h"

namespace brisk {

	struct Expr;
	class BriskParser;

	struct VarDeclParser : public Parser
	{
		std::unique_ptr<Expr> parse(BriskParser &parser) override;
	};
}