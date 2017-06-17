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

	struct Expr;

	struct Symbol
	{
		inline virtual ~Symbol() {}
		virtual SymbolType type() const = 0;
		virtual std::string symbol_type_name() const = 0;
		virtual const Type *expr_type() = 0;
		virtual const Expr *expr() = 0;
	};

	struct VarDeclExpr;
	struct FnDeclExpr;
	struct FnArgExpr;

	struct VarSymbol : public Symbol
	{
		inline VarSymbol(VarDeclExpr *expr) : decl_expr(expr) {}
		VarDeclExpr *decl_expr;

		inline SymbolType type() const override
		{
			return SymbolType::Var;
		}

		inline std::string symbol_type_name() const override
		{
			return "variable";
		}

		const Type *expr_type() override;
		const Expr *expr() override;
	};

	struct FnSymbol : public Symbol
	{
		inline FnSymbol(FnDeclExpr *expr) : decl_expr(expr) {}
		FnDeclExpr *decl_expr;

		inline SymbolType type() const override
		{
			return SymbolType::Fn;
		}

		inline std::string symbol_type_name() const override
		{
			return "function";
		}

		const Type *expr_type() override;
		const Expr *expr() override;
	};

	struct FnArgSymbol : public Symbol
	{
		inline FnArgSymbol(FnArgExpr *expr) : arg_expr(expr) {}
		FnArgExpr *arg_expr;

		inline SymbolType type() const override
		{
			return SymbolType::FnArg;
		}

		inline std::string symbol_type_name() const override
		{
			return "function argument";
		}

		const Type *expr_type() override;
		const Expr *expr() override;
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
		Symbol *find_base(const std::string &name);
		template<class TSymbol>
		inline TSymbol *find(const std::string &name)
		{
			static_assert(std::is_base_of<Symbol, TSymbol>::value, "TSymbol must inherit from Symbol");
			auto symbol = find_base(name);
			if (symbol == nullptr)
				return nullptr;

			return static_cast<TSymbol*>(symbol);
		}

	private:
		void validate_add(const Expr *expr, const std::string &name);

		std::unordered_map<std::string, std::unique_ptr<Symbol>> symbols_;
		SymbolTable *parent_;
	};

}
