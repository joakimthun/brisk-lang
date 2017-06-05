#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "../typedef.h"
#include "../type.h"

namespace brisk {

	enum class SymbolType : u8
	{
		Var,
		Fn,
		FnArg
	};

	struct Symbol
	{
		inline virtual ~Symbol() {}
		virtual SymbolType type() = 0;
		virtual const Type *expr_type() = 0;
	};

	struct VarDeclExpr;
	struct FnDeclExpr;
	struct FnArgExpr;

	struct VarSymbol : public Symbol
	{
		inline VarSymbol(VarDeclExpr *expr) : expr(expr) {}
		VarDeclExpr *expr;

		inline SymbolType type() override
		{
			return SymbolType::Var;
		}

		const Type *expr_type() override;
	};

	struct FnSymbol : public Symbol
	{
		inline FnSymbol(FnDeclExpr *expr) : expr(expr) {}
		FnDeclExpr *expr;

		inline SymbolType type() override
		{
			return SymbolType::Fn;
		}

		const Type *expr_type() override;
	};

	struct FnArgSymbol : public Symbol
	{
		inline FnArgSymbol(FnArgExpr *expr) : expr(expr) {}
		FnArgExpr *expr;

		inline SymbolType type() override
		{
			return SymbolType::FnArg;
		}

		const Type *expr_type() override;
	};

	class SymbolTable
	{
	public:
		SymbolTable(SymbolTable *parent);

		bool root();
		SymbolTable *parent();
		void add_fn(FnDeclExpr *expr);
		void add_fn_arg(FnArgExpr *expr);
		void add_var(VarDeclExpr *expr);

		template<class TSymbol>
		inline TSymbol *find(const std::string &name)
		{
			static_assert(std::is_base_of<Symbol, TSymbol>::value, "TComponent must inherit from Symbol");

			auto it = symbols_.find(name);
			if (it != symbols_.end())
			{
				return static_cast<TSymbol*>(it->second.get());
			}

			if (root())
				return nullptr;

			return parent_->find<TSymbol>(name);
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<Symbol>> symbols_;
		SymbolTable *parent_;
	};

}
