#pragma once

#include "parser.h"

namespace brisk {

	struct FnDeclParser : public Parser
	{
		FnDeclParser();
		FnDeclParser(bool ext);

		std::unique_ptr<Expr> parse(BriskParser &parser) override;

	private:
		bool ext_;
	};

}
