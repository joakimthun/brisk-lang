#pragma once

#include "ast.h"

namespace brisk {

	struct ASTVisitor
	{
		inline virtual void visit(Ast &ast) 
		{
			for (auto& e : ast.exprs)
				e->accept(*this);
		}

		inline virtual void visit(BinExpr &expr) {}
		inline virtual void visit(LiteralExpr &expr) {}
		inline virtual void visit(IdentifierExpr &expr) {}
		inline virtual void visit(AssignExpr &expr) {}
		inline virtual void visit(FnDeclExpr &expr) {}
		inline virtual void visit(RetExpr &expr) {}
		inline virtual void visit(VarDeclExpr &expr) {}
		inline virtual void visit(FnCallExpr &expr) {}
		inline virtual void visit(IfExpr &expr) {}
	};
}
