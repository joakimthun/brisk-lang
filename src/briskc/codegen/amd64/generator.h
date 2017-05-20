#pragma once

#include "ast/ast_visitor.h"
#include "emitter.h"

namespace brisk {
	namespace x64 {

		class Generator : public ASTVisitor
		{
		public:
			Generator();

			void visit(Ast &ast) override;
			void visit(BinExpr &expr) override;
			void visit(LiteralExpr &expr) override;
			void visit(IdentifierExpr &expr) override;
			void visit(AssignExpr &expr) override;
			void visit(BlockExpr &expr) override;
			void visit(FnDeclExpr &expr) override;
			void visit(RetExpr &expr) override;

		private:
			Emitter emitter_;
		};

	}
}
