#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../token_type.h"
#include "../token.h"
#include "../type.h"
#include "../string_view.h"

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

	struct Ast
	{
		std::string file;
		std::vector<std::unique_ptr<Node>> exprs;
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

	struct BlockExpr : public Expr
	{
		std::vector<std::unique_ptr<Node>> exprs;

		void accept(ASTVisitor &visitor) override;
	};

	struct FnDeclExpr : public Expr
	{
		StringView name;
		std::unique_ptr<BlockExpr> body;

		void accept(ASTVisitor &visitor) override;
	};

	struct RetExpr : public Expr
	{
		std::unique_ptr<Expr> expr;

		void accept(ASTVisitor &visitor) override;
	};
}
