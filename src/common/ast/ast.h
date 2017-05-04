#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../token_type.h"
#include "../token.h"
#include "../type.h"

namespace brisk {

	struct Node
	{
		inline virtual ~Node() {};
		Token start;
		Token end;
	};

	struct Expr : public Node
	{
		inline virtual ~Expr() {};
	};

	struct Stmt : public Node
	{
		inline virtual ~Stmt() {};
	};

	struct Ast
	{
		std::string file;
		std::vector <std::unique_ptr<Node>> stmts;
	};

	struct BinExpr : public Expr
	{
		std::unique_ptr<Expr> left;
		std::unique_ptr<Expr> right;
		TokenType op;
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
	};
}
