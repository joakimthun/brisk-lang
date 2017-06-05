#pragma once

#include <string>
#include <vector>

#include "ast/ast_visitor.h"
#include "type.h"

namespace brisk {

	struct TypeError
	{

	};

	class TypeChecker : public ASTVisitor
	{
	public:
		TypeChecker();

		void visit(Ast &ast) override;
		void visit(BinExpr &expr) override;
		void visit(IdentifierExpr &expr) override;
		void visit(AssignExpr &expr) override;
		void visit(FnDeclExpr &expr) override;
		void visit(RetExpr &expr) override;
		void visit(VarDeclExpr &expr) override;
		void visit(FnCallExpr &expr) override;

	private:
		//void 

		std::vector<TypeError> type_errors_;
	};

}
