#include "type_checker.h"

#include <algorithm>
#include <sstream>

#include "ast/ast.h"
#include "brisk_exception.h"
#include "../exceptions/type_exception.h"
#include "type_converters.h"

namespace brisk {

	TypeChecker::TypeChecker(TypeTable &type_table) 
		:
		type_table_(type_table)
	{
	}

	void TypeChecker::visit(Ast &ast)
	{
		for (auto& e : ast.exprs)
			e->accept(*this);

		throw_if_max_errors_reached(1);
	}

	void TypeChecker::visit(BinExpr &expr)
	{
		expr.left->accept(*this);
		expr.right->accept(*this);

		auto left_type = expr.left->type;
		auto right_type = expr.right->type;

		if (left_type->id() != right_type->id())
			register_type_error(expr.start, "Operator '" + operator_from_token(expr.op) + "' cannot be applied to operands of type '" + expr.left->type->name() + "' and '" + expr.right->type->name() + "'");
	}

	void TypeChecker::visit(IdentifierExpr &expr)
	{

	}

	void TypeChecker::visit(AssignExpr &expr)
	{
		// TODO: The parser makes sure left is an IdentifierExpr right now, but will not in the future
		const auto identifier = static_cast<IdentifierExpr*>(expr.left.get());
		if (!identifier->mut)
		{
			register_type_error(identifier->start, "'" + identifier->name.to_string() + "' cannot be modified since it's not a mutable value");
		}

		expr.left->accept(*this);
		expr.right->accept(*this);

		auto left_type = expr.left->type;
		auto right_type = expr.right->type;

		if (left_type->id() != right_type->id())
			register_type_error(expr.start, "Cannot convert from '" + expr.right->type->name() + "' to '" + expr.left->type->name() + "'");
	}

	void TypeChecker::visit(FnDeclExpr &expr)
	{
		for (auto& e : expr.body)
			e->accept(*this);
	}

	void TypeChecker::visit(RetExpr &expr)
	{
		expr.expr->accept(*this);

		if (!check_expr(*expr.expr, expr.owner->return_type))
			register_type_error(expr.start, "Cannot convert return value from '" + expr.expr->type->name() + "' to '" + expr.owner->return_type->name() + "'");
	}

	void TypeChecker::visit(VarDeclExpr &expr)
	{
		expr.expr->accept(*this);

		if (!expr.type->equals(expr.expr->type))
			register_type_error(expr.start, "Cannot convert from '" + expr.expr->type->name() + "' to '" + expr.type->name() + "'");
	}

	void TypeChecker::visit(FnCallExpr &expr)
	{
		check_fn_call(expr);
	}

	void TypeChecker::visit(IfExpr &expr)
	{
		expr.if_condition->accept(*this);
		for(auto& e : expr.if_body)
			e->accept(*this);

		const auto bool_type = type_table_.get("bool", false);
		if (!expr.if_condition->type->equals(bool_type))
			register_type_error(expr.start, "Non-bool type used as if condition. Cannot convert from '" + expr.if_condition->type->name() + "' to '" + bool_type->name() + "'");
	}

	void TypeChecker::check_fn_call(const FnCallExpr &expr)
	{
		const auto& callee_expr = expr.callee->decl_expr;

		if (callee_expr.args.size() != expr.args.size())
		{
			if(expr.args.size() > callee_expr.args.size())
				register_type_error(expr.start, "'" + callee_expr.name.to_string() + "' was called with to many arguments");

			if (expr.args.size() < callee_expr.args.size())
				register_type_error(expr.start, "'" + callee_expr.name.to_string() + "' was called with to few arguments");
		}

		auto min = std::min(callee_expr.args.size(), expr.args.size());
		for (auto i = 0; i < min; i++)
			check_fn_arg(*callee_expr.args[i], *expr.args[i], i);
	}

	void TypeChecker::check_fn_arg(const FnArgExpr &arg, Expr &expr, int index)
	{
		if (!check_expr(expr, arg.type))
			register_type_error(expr.start, "Argument " + std::to_string(index) + ": cannot convert from '" + expr.type->name() + "' to '" + arg.type->name() + "'");
	}

	void TypeChecker::register_type_error(const Token &location, const std::string &message)
	{
		std::ostringstream ss;
		ss << message << std::endl;
		ss << "Row: " << location.row << " Column: " << location.column_start << "-" << location.column_end << std::endl;
		ss << "File: '" << location.file->path << "'" << std::endl;

		type_errors_.push_back(ss.str());

		throw_if_max_errors_reached(5);
	}

	void TypeChecker::throw_if_max_errors_reached(u16 max_errors)
	{
		if (type_errors_.size() >= max_errors)
			throw TypeException(type_errors_);
	}

	bool TypeChecker::check_expr(Expr &expr, const Type* target)
	{
		if (!expr.type->equals(target))
		{
			return types::try_static_convert(expr, target, type_table_);
		}

		return true;
	}
}
