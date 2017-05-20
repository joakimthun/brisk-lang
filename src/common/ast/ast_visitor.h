#pragma once

#include "ast.h"

namespace brisk {

	struct ASTVisitor
	{
		inline virtual void visit(Ast &ast) {}
		inline virtual void visit(BinExpr &expr) {}
		inline virtual void visit(LiteralExpr &expr) {}
		inline virtual void visit(IdentifierExpr &expr) {}
		inline virtual void visit(AssignExpr &expr) {}
		inline virtual void visit(BlockExpr &expr) {}
		inline virtual void visit(FnDeclExpr &expr) {}
		inline virtual void visit(RetExpr &expr) {}
	};
}
