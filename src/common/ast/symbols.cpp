#include "symbols.h"

#include "ast.h"

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
		// TODO: Validate unique
		symbols_.insert(sym_entry(expr->name.to_string(), std::make_unique<FnSymbol>(expr)));
	}

	void SymbolTable::add_fn_arg(FnArgExpr *expr)
	{
		// TODO: Validate unique
		symbols_.insert(sym_entry(expr->name.to_string(), std::make_unique<FnArgSymbol>(expr)));
	}

	void SymbolTable::add_var(VarDeclExpr *expr)
	{
		// TODO: Validate unique
		symbols_.insert(sym_entry(expr->name.to_string(), std::make_unique<VarSymbol>(expr)));
	}

	const Type *VarSymbol::expr_type()
	{
		return expr->type;
	}

	const Type *FnSymbol::expr_type()
	{
		return expr->type;
	}

	const Type *FnArgSymbol::expr_type()
	{
		return expr->type;
	}

}
