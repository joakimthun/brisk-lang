#pragma once

#include <string>
#include <vector>

#include "typedef.h"
#include "ast/ast_visitor.h"
#include "token.h"
#include "type.h"
#include "file.h"
#include "type_table.h"

namespace brisk {

	class TypeChecker : public ASTVisitor
	{
	public:
		TypeChecker(TypeTable &type_table);

		void visit(Ast &ast) override;
		void visit(BinExpr &expr) override;
		void visit(IdentifierExpr &expr) override;
		void visit(AssignExpr &expr) override;
		void visit(FnDeclExpr &expr) override;
		void visit(RetExpr &expr) override;
		void visit(VarDeclExpr &expr) override;
		void visit(FnCallExpr &expr) override;
		void visit(IfExpr &expr) override;

	private:
		void check_fn_call(const FnCallExpr &expr);
		void check_fn_arg(const FnArgExpr &arg, Expr &expr, int index);
		void register_type_error(const Token &location, const std::string &message);
		void throw_if_max_errors_reached(u16 max_errors);
		bool check_expr(Expr &expr, const Type *target);

		TypeTable &type_table_;
		std::vector<std::string> type_errors_;
	};

}
