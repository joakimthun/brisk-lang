#pragma once

#include <string>
#include <vector>

#include "typedef.h"
#include "ast/ast_visitor.h"
#include "token.h"
#include "type.h"
#include "file.h"

namespace brisk {

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
		void check_fn_call(const FnCallExpr &expr);
		void check_fn_arg(const FnArgExpr &arg, const Expr &expr, int index);
		void register_type_error(const Token &location, const std::string &message);
		void throw_if_max_errors_reached(u16 max_errors);

		std::vector<std::string> type_errors_;
	};

}
