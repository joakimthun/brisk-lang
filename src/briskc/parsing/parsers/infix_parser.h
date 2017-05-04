#pragma once

#include <memory>

#include "typedef.h"
#include "../precedence.h"

namespace brisk {

	struct Expr;
	class BriskParser;

	class InfixParser
	{
	public:
		virtual ~InfixParser() {};
		virtual std::unique_ptr<Expr> parse(BriskParser &parser, std::unique_ptr<Expr> left) = 0;
		virtual u8 precedence() = 0;
	};
}