#pragma once

#include "infix_parser.h"

namespace brisk {

	class BinExprParser : public InfixParser
	{
	public:
		BinExprParser(Precedence precedence);

		std::unique_ptr<Expr> parse(BriskParser &parser, std::unique_ptr<Expr> left) override;
		u8 precedence() override;

	private:
		Precedence precedence_;
	};

}