#include "symbols.h"

#include <sstream>

#include "ast.h"
#include "../file.h"
#include "../brisk_exception.h"

namespace brisk {

	typedef std::pair<std::string, std::unique_ptr<Symbol>> sym_entry;

	SymbolTable::SymbolTable(SymbolTable *parent)
		:
		parent_(parent)
	{
	}

	bool SymbolTable::root()
	{
		return parent_ == nullptr;
	}

	SymbolTable *SymbolTable::parent()
	{
		return parent_;
	}

	void SymbolTable::add_fn(FnDeclExpr *expr)
	{
		validate_add(expr, expr->name.to_string());
		symbols_.insert(sym_entry(expr->name.to_string(), std::make_unique<FnSymbol>(expr)));
	}

	void SymbolTable::add_fn_arg(FnArgExpr *expr)
	{
		validate_add(expr, expr->name.to_string());
		symbols_.insert(sym_entry(expr->name.to_string(), std::make_unique<FnArgSymbol>(expr)));
	}

	void SymbolTable::add_var(VarDeclExpr *expr)
	{
		validate_add(expr, expr->name.to_string());
		symbols_.insert(sym_entry(expr->name.to_string(), std::make_unique<VarSymbol>(expr)));
	}

	Symbol *SymbolTable::find_base(const std::string &name)
	{
		auto it = symbols_.find(name);
		if (it != symbols_.end())
		{
			return it->second.get();
		}

		if (root())
			return nullptr;

		return parent_->find_base(name);
	}

	void SymbolTable::validate_add(const Expr *expr, const std::string &name)
	{
		auto duplicate = find_base(name);
		if (duplicate == nullptr)
			return;

		const auto &this_location = expr->start;
		const auto &duplicate_location = duplicate->expr()->start;
		std::ostringstream ss;
		ss << "Row: " << this_location.row << " Column: " << this_location.column_start << "-" << this_location.column_end << std::endl;
		ss << "File: '" << this_location.file->path << "'" << std::endl;
		ss << "A " + duplicate->symbol_type_name() + " with the name '" + name + "' has already been declared in the current scope at:" << std::endl;
		ss << "Row: " << duplicate_location.row << " Column: " << duplicate_location.column_start << "-" << duplicate_location.column_end << std::endl;
		ss << "File: '" << duplicate_location.file->path << "'" << std::endl;
		
		throw BriskException(ss.str());
	}

	const Type *VarSymbol::expr_type()
	{
		return decl_expr->type;
	}

	const Expr *VarSymbol::expr()
	{
		return decl_expr;
	}

	const Type *FnSymbol::expr_type()
	{
		return decl_expr->type;
	}

	const Expr *FnSymbol::expr()
	{
		return decl_expr;
	}

	const Type *FnArgSymbol::expr_type()
	{
		return arg_expr->type;
	}

	const Expr *FnArgSymbol::expr()
	{
		return arg_expr;
	}
}
