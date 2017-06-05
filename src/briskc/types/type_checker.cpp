#include "type_checker.h"

#include "ast/ast.h"
#include "brisk_exception.h"

namespace brisk {

	TypeChecker::TypeChecker() {}

	void TypeChecker::visit(Ast &ast)
	{
		for (auto& e : ast.exprs)
			e->accept(*this);
	}

	void TypeChecker::visit(BinExpr &expr)
	{
		expr.left->accept(*this);
		expr.right->accept(*this);

		auto left_type = expr.left->type;
		auto right_type = expr.right->type;

		if (left_type->id() != right_type->id())
			throw BriskException("BinExpr: left_type->id != right_type->id");
	}

	void TypeChecker::visit(IdentifierExpr &expr)
	{

	}

	void TypeChecker::visit(AssignExpr &expr)
	{
		expr.left->accept(*this);
		expr.right->accept(*this);

		auto left_type = expr.left->type;
		auto right_type = expr.right->type;

		if (left_type->id() != right_type->id())
			throw BriskException("AssignExpr: left_type->id != right_type->id");
	}

	void TypeChecker::visit(FnDeclExpr &expr)
	{
		for (auto& e : expr.body)
			e->accept(*this);
	}

	void TypeChecker::visit(RetExpr &expr)
	{

	}

	void TypeChecker::visit(VarDeclExpr &expr)
	{
		expr.expr->accept(*this);
	}

	void TypeChecker::visit(FnCallExpr &expr)
	{

	}
}
