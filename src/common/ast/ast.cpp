#include "ast.h"

#include "ast_visitor.h"

namespace brisk {

	void BinExpr::accept(ASTVisitor &visitor)
	{
		visitor.visit(*this);
	}

	void LiteralExpr::accept(ASTVisitor &visitor)
	{
		visitor.visit(*this);
	}

	void IdentifierExpr::accept(ASTVisitor &visitor)
	{
		visitor.visit(*this);
	}

	void AssignExpr::accept(ASTVisitor &visitor)
	{
		visitor.visit(*this);
	}

	void FnDeclExpr::accept(ASTVisitor &visitor)
	{
		visitor.visit(*this);
	}

	void RetExpr::accept(ASTVisitor &visitor)
	{
		visitor.visit(*this);
	}	

	void VarDeclExpr::accept(ASTVisitor &visitor)
	{
		visitor.visit(*this);
	}

	void FnCallExpr::accept(ASTVisitor &visitor)
	{
		visitor.visit(*this);
	}

}
