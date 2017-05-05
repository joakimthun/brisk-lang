#pragma once

#include "infix_parser.h"

namespace brisk {

	struct AssignmentParser : public InfixParser
	{
		std::unique_ptr<Expr> parse(BriskParser &parser, std::unique_ptr<Expr> left) override;
		u8 precedence() override;
	};

}