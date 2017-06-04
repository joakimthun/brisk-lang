#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "ast/ast_visitor.h"
#include "type.h"

namespace brisk {
	namespace x64 {

		class TypeChecker : public ASTVisitor
		{
		public:
			TypeChecker();

			void visit(Ast &ast) override;
			void visit(BinExpr &expr) override;
			void visit(LiteralExpr &expr) override;
			void visit(IdentifierExpr &expr) override;
			void visit(AssignExpr &expr) override;
			void visit(FnDeclExpr &expr) override;
			void visit(RetExpr &expr) override;
			void visit(VarDeclExpr &expr) override;
			void visit(FnCallExpr &expr) override;
		private:
			
		};

	}
}
