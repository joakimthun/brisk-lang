#pragma once

#include "infix_parser.h"

namespace brisk {

	struct BinExprParser : public InfixParser
	{
		BinExprParser(Precedence precedence);
		std::unique_ptr<Expr> parse(BriskParser &parser, std::unique_ptr<Expr> left) override;
		u8 precedence() override;

	private:
		Precedence precedence_;
	};

}