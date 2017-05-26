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

	struct Node
	{
		inline virtual ~Node() {};
		virtual void accept(ASTVisitor &visitor) = 0;
		Token start;
		Token end;
	};

	struct Expr : public Node
	{
		inline virtual ~Expr() {};
	};

	struct Block : public Expr
	{
		inline Block(SymbolTable *parent) : symbol_table(parent) {}
		inline virtual ~Block() {};
		SymbolTable symbol_table;
	};

	struct Ast
	{
		inline Ast() : symbol_table(nullptr) {}

		std::string file;
		std::vector<std::unique_ptr<Node>> exprs;
		SymbolTable symbol_table;
	};

	struct BinExpr : public Expr
	{
		std::unique_ptr<Expr> left;
		std::unique_ptr<Expr> right;
		TokenType op;

		void accept(ASTVisitor &visitor) override;
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
		Type type;

		void accept(ASTVisitor &visitor) override;
	};

	struct IdentifierExpr : public Expr
	{
		StringView name;

		void accept(ASTVisitor &visitor) override;
	};

	struct AssignExpr : public Expr
	{
		std::unique_ptr<Expr> left;
		std::unique_ptr<Expr> right;

		void accept(ASTVisitor &visitor) override;
	};

	struct FnArg
	{
		inline FnArg(const StringView &name, const Type &type) : name(name), type(type) {}
		StringView name;
		Type type;
	};

	struct FnDeclExpr : public Block
	{
		inline FnDeclExpr(SymbolTable *parent) : Block(parent) {}

		StringView name;
		Type return_type;
		std::vector<std::unique_ptr<FnArg>> args;
		std::vector<std::unique_ptr<Expr>> body;

		void accept(ASTVisitor &visitor) override;
	};

	struct RetExpr : public Expr
	{
		std::unique_ptr<Expr> expr;

		void accept(ASTVisitor &visitor) override;
	};

	struct VarDeclExpr : public Expr
	{
		bool mut = false;
		StringView name;
		std::unique_ptr<Expr> expr;

		void accept(ASTVisitor &visitor) override;
	};

	struct FnCallExpr : public Expr
	{
		StringView name;
		std::vector<std::unique_ptr<Expr>> args;

		void accept(ASTVisitor &visitor) override;
	};
}
