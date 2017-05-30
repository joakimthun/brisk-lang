#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <stack>

#include "ast/ast_visitor.h"
#include "../coff/coff.h"
#include "../coff/coff_writer.h"
#include "emitter.h"
#include "register_allocator.h"
#include "stack_allocator.h"
#include "string_view.h"
#include "byte_buffer.h"
#include "../addr_table.h"

namespace brisk {
	namespace x64 {

		struct AddedSymbolInfo
		{
			u32 sym_table_index;
		};

		typedef std::pair<std::string, AddedSymbolInfo> added_sym_pair;
		typedef std::pair<bool, u32> find_symbol_result;

		class Generator : public ASTVisitor
		{
		public:
			Generator();

			void visit(Ast &ast) override;
			void visit(BinExpr &expr) override;
			void visit(LiteralExpr &expr) override;
			void visit(IdentifierExpr &expr) override;
			void visit(AssignExpr &expr) override;
			void visit(FnDeclExpr &expr) override;
			void visit(RetExpr &expr) override;
			void visit(VarDeclExpr &expr) override;
			void visit(FnCallExpr &expr) override;

			void write_to_disk(const std::string &path);
		private:
			void store_fn_args(FnDeclExpr &expr);

			u32 add_fn_symbol(StringView &name, u32 value);
			u32 add_ext_fn_symbol(const StringView &name);
			u32 add_static_data_symbol(u32 value);
			void add_rel_reloc(u32 vaddr, u32 symndx);
			find_symbol_result find_added_symbol(const StringView &name);
			void add_addr_entry(const StringView &name, u64 sp_rel_addr);
			u64 find_addr_entry(const StringView &name);
			void push_addr_table();
			void pop_addr_table();

			Emitter emitter_;
			std::unique_ptr<ByteBuffer> data_;
			RegisterAllocator reg_allocator_;
			StackAllocator stack_allocator_;
			coff::CoffWriter coff_writer_;
			std::unordered_map<std::string, AddedSymbolInfo> added_symbols_;
			std::stack<std::unique_ptr<AddrTable>> addr_tables_;
		};

	}
}
