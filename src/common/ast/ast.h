#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../token_type.h"
#include "../token.h"
#include "../type.h"
#include "../string_view.h"
#include "symbols.h"

namespace brisk {

	struct ASTVisitor;
	struct FnDeclExpr;

	enum class NodeType : u16
	{
		Block,
		Bin,
		Literal,
		Identifier,
		Assign,
		FnArg,
		FnDecl,
		Ret,
		VarDecl,
		FnCall
	};

	struct Expr
	{
		inline virtual ~Expr() {};
		virtual void accept(ASTVisitor &visitor) = 0;
		virtual NodeType node_type() const = 0;
		template<class TExpr>
		inline bool is() const
		{
			static_assert(std::is_base_of<Expr, TExpr>::value, "TExpr must inherit from Expr");
			return dynamic_cast<TExpr*>(this) != nullptr;
		}

		template<class TExpr>
		inline TExpr *as()
		{
			static_assert(std::is_base_of<Expr, TExpr>::value, "TExpr must inherit from Expr");
			return dynamic_cast<TExpr*>(this);
		}

		Token start;
		Token end;
		const Type *type;
		const FnDeclExpr *owner;
	};

	struct Block : public Expr
	{
		inline Block(SymbolTable *parent) : symbol_table(parent) {}
		inline virtual ~Block() {};
		NodeType node_type() const override
		{
			return NodeType::Block;
		}

		SymbolTable symbol_table;
	};

	struct Ast
	{
		inline Ast() : symbol_table(nullptr) {}

		std::string file;
		StringView pkg_name;
		std::vector<std::unique_ptr<Expr>> exprs;
		SymbolTable symbol_table;
	};

	struct BinExpr : public Expr
	{
		std::unique_ptr<Expr> left;
		std::unique_ptr<Expr> right;
		TokenType op;

		void accept(ASTVisitor &visitor) override;
		NodeType node_type() const override
		{
			return NodeType::Bin;
		}
	};

	struct LiteralExpr : public Expr
	{
		union 
		{
			u8 u8;
			i8 i8;
			u16 u16;
			i16 i16;
			u32 u32;
			i32 i32;
			u64 u64;
			i64 i64;
			float f;
			double d;
		} value;
		StringView str_value;

		void accept(ASTVisitor &visitor) override;
		NodeType node_type() const override
		{
			return NodeType::Literal;
		}
	};

	struct IdentifierExpr : public Expr
	{
		StringView name;
		bool mut = false;

		void accept(ASTVisitor &visitor) override;
		NodeType node_type() const override
		{
			return NodeType::Identifier;
		}
	};

	struct AssignExpr : public Expr
	{
		std::unique_ptr<Expr> left;
		std::unique_ptr<Expr> right;

		void accept(ASTVisitor &visitor) override;
		NodeType node_type() const override
		{
			return NodeType::Assign;
		}
	};

	struct FnArgExpr : public Expr
	{
		StringView name;

		inline void accept(ASTVisitor &visitor) override {}
		NodeType node_type() const override
		{
			return NodeType::FnArg;
		}
	};

	struct FnDeclExpr : public Block
	{
		inline FnDeclExpr(SymbolTable *parent, bool ext) : ext(ext), Block(parent) {}

		StringView name;
		const Type *return_type;
		std::vector<std::unique_ptr<FnArgExpr>> args;
		std::vector<std::unique_ptr<Expr>> body;
		bool ext;

		void accept(ASTVisitor &visitor) override;
		NodeType node_type() const override
		{
			return NodeType::FnDecl;
		}
	};

	struct RetExpr : public Expr
	{
		std::unique_ptr<Expr> expr;

		void accept(ASTVisitor &visitor) override;
		NodeType node_type() const override
		{
			return NodeType::Ret;
		}
	};

	struct VarDeclExpr : public Expr
	{
		bool mut = false;
		StringView name;
		std::unique_ptr<Expr> expr;

		void accept(ASTVisitor &visitor) override;
		NodeType node_type() const override
		{
			return NodeType::VarDecl;
		}
	};

	struct FnCallExpr : public Expr
	{
		StringView name;
		const FnType *callee;
		std::vector<std::unique_ptr<Expr>> args;

		void accept(ASTVisitor &visitor) override;
		NodeType node_type() const override
		{
			return NodeType::FnCall;
		}
	};
}
