#pragma once

#include <memory>

namespace brisk {

	class BriskParser;
	struct Expr;
	
	class Parser
	{
	public:
		virtual ~Parser() {};
		virtual std::unique_ptr<Expr> parse(BriskParser &parser) = 0;
	};
	
}