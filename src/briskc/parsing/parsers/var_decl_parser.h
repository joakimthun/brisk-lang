#pragma once

#include <memory>

#include "parser.h"

namespace brisk {

	struct Expr;
	class BriskParser;

	struct VarDeclParser : public Parser
	{
		VarDeclParser(bool explicit_type);
		std::unique_ptr<Expr> parse(BriskParser &parser) override;

	private:
		bool explicit_type_;
	};
}