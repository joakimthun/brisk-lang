#include "generator.h"

#include <iostream>

#include "ast/ast.h"

namespace brisk {
	namespace x64 {

		Generator::Generator()
		{
		}

		void Generator::visit(Ast &ast)
		{
			std::cout << "Ast" << std::endl;

			for (auto& e : ast.exprs)
				e->accept(*this);
		}

		void Generator::visit(BinExpr &expr)
		{
			std::cout << "BinExpr" << std::endl;
			expr.left->accept(*this);
			expr.right->accept(*this);
		}

		void Generator::visit(LiteralExpr &expr)
		{
			std::cout << "LiteralExpr" << std::endl;
		}

		void Generator::visit(IdentifierExpr &expr)
		{
			std::cout << "IdentifierExpr" << std::endl;
		}

		void Generator::visit(AssignExpr &expr)
		{
			std::cout << "AssignExpr" << std::endl;
		}

		void Generator::visit(BlockExpr &expr)
		{
			std::cout << "BlockExpr" << std::endl;
			for (auto& e : expr.exprs)
				e->accept(*this);
		}

		void Generator::visit(FnDeclExpr &expr)
		{
			std::cout << "FnDeclExpr" << std::endl;
			expr.body->accept(*this);
		}

		void Generator::visit(RetExpr &expr)
		{
			std::cout << "RetExpr" << std::endl;
			expr.expr->accept(*this);
		}

	}
}
