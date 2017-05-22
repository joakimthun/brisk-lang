#pragma once

#include <string>
#include <memory>

#include "ast/ast_visitor.h"
#include "../coff/coff.h"
#include "../coff/coff_writer.h"
#include "emitter.h"
#include "register_allocator.h"
#include "string_view.h"
#include "byte_buffer.h"

namespace brisk {
	namespace x64 {

		class Generator : public ASTVisitor
		{
		public:
			Generator();

			void visit(Ast &ast) override;
			void visit(BinExpr &expr) override;
			void visit(LiteralExpr &expr) override;
			void visit(IdentifierExpr &expr) override;
			void visit(AssignExpr &expr) override;
			void visit(BlockExpr &expr) override;
			void visit(FnDeclExpr &expr) override;
			void visit(RetExpr &expr) override;
			void visit(VarDeclExpr &expr) override;
			void visit(FnCallExpr &expr) override;

			void write_to_disk(const std::string &path);
		private:
			void add_ext_fn_symbol(StringView &name);

			Emitter emitter_;
			std::unique_ptr<ByteBuffer> data_;
			RegisterAllocator reg_allocator_;
			coff::CoffWriter coff_writer_;
		};

	}
}
