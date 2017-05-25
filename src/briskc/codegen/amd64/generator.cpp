#include "generator.h"

#include <iostream>

#include "ast/ast.h"

namespace brisk {
	namespace x64 {

		Generator::Generator()
			:
			coff_writer_(coff::MACHINE::IMAGE_FILE_MACHINE_AMD64),
			data_(std::make_unique<ByteBuffer>()),
			stack_allocator_(emitter_)
		{
			coff_writer_.add_section(".data", coff::SectionHeaderFlags::STYP_DATA, nullptr);
			coff_writer_.add_section(".code", coff::SectionHeaderFlags::STYP_TEXT, nullptr);
			addr_tables_.push(std::make_unique<AddrTable>(nullptr));
		}

		void Generator::visit(Ast &ast)
		{
			std::cout << "Ast" << std::endl;

			for (auto& e : ast.exprs)
				e->accept(*this);
		}

		void Generator::visit(BinExpr &expr)
		{
			expr.left->accept(*this);
			auto left_reg = reg_allocator_.pop();
			expr.right->accept(*this);
			auto right_reg = reg_allocator_.pop();

			emitter_.emit_add(left_reg, right_reg);
			reg_allocator_.push(right_reg);
		}

		void Generator::visit(LiteralExpr &expr)
		{
			if (expr.type.id == TypeID::U8 && expr.type.ptr)
			{
				auto reg = reg_allocator_.get_free();
				emitter_.emit_lea64(Register::RCX, 0);
				auto symbol_index = add_static_data_symbol(data_->length());
				data_->write(expr.str_value.data(), expr.str_value.length());
				data_->write(0);

				// Subtract the 4 byte displacement from the buffer offset
				add_rel_reloc(emitter_.current_buffer_offset() - 4, symbol_index);

				reg_allocator_.push(reg);
			}
			else
			{
				auto dest_reg = reg_allocator_.get_free();
				emitter_.emit_mov(dest_reg, expr.value.i32);
				reg_allocator_.push(dest_reg);
			}
		}

		void Generator::visit(IdentifierExpr &expr)
		{
			auto rsp_rel_addr = find_addr_entry(expr.name);
			auto dest_reg = reg_allocator_.get_free();
			emitter_.emit_spd_mov(dest_reg, rsp_rel_addr);
			reg_allocator_.push(dest_reg);
		}

		void Generator::visit(AssignExpr &expr)
		{
			std::cout << "AssignExpr" << std::endl;
		}

		void Generator::visit(FnDeclExpr &expr)
		{
			push_addr_table();
			add_fn_symbol(expr.name, emitter_.current_buffer_offset());
			stack_allocator_.reserve(expr);
			stack_allocator_.reset();

			for(auto& e : expr.body)
				e->accept(*this);

			stack_allocator_.free(expr);
			emitter_.emit_ret();
			pop_addr_table();
		}

		void Generator::visit(RetExpr &expr)
		{
			expr.expr->accept(*this);

			auto src_reg = reg_allocator_.pop();
			// Just move the return value into RAX/EAX, ret will be emitted when emitting the function
			emitter_.emit_mov(Register::EAX, src_reg);
		}

		void Generator::visit(VarDeclExpr &expr)
		{
			expr.expr->accept(*this);
			auto value_reg = reg_allocator_.pop();
			auto sp_rel_addr = stack_allocator_.reserve(4);

			emitter_.emit_spd_mov(sp_rel_addr, value_reg);
			add_addr_entry(expr.name, sp_rel_addr);
		}

		void Generator::visit(FnCallExpr &expr)
		{
			// Integer arguments are passed in registers RCX, RDX, R8, and R9. Floating point arguments are passed in XMM0L, XMM1L, XMM2L, and XMM3L.
			// In addition to these registers, RAX, R10, R11, XMM4, and XMM5 are considered volatile. 
			for (auto& arg : expr.args)
				arg->accept(*this);

			emitter_.emit_call();

			// Subtract the 4 byte displacement from the buffer offset
			auto symbol_index = add_ext_fn_symbol(expr.name);
			add_rel_reloc(emitter_.current_buffer_offset() - 4, symbol_index);
		}

		void Generator::write_to_disk(const std::string &path)
		{
			coff_writer_.set_section_content(".data", std::move(data_));
			coff_writer_.set_section_content(".code", emitter_.buffer());
			coff_writer_.write_to_disk(path);
		}

		u32 Generator::add_fn_symbol(StringView &name, u32 value)
		{
			auto result = find_added_symbol(name);
			if (result.first)
				return result.second;

			return coff_writer_.add_symbol(name.to_string(), 0x2, coff::SymbolTableMsEntryType::IMAGE_SYM_DTYPE_FUNCTION, coff::SymbolTableEntryClass::IMAGE_SYM_CLASS_EXTERNAL, value);
		}

		u32 Generator::add_ext_fn_symbol(const StringView &name)
		{
			auto result = find_added_symbol(name);
			if (result.first)
				return result.second;

			return coff_writer_.add_symbol(name.to_string(), 0x0, coff::SymbolTableMsEntryType::IMAGE_SYM_DTYPE_FUNCTION, coff::SymbolTableEntryClass::IMAGE_SYM_CLASS_EXTERNAL);
		}

		u32 Generator::add_static_data_symbol(u32 value)
		{
			static auto i = 0;
			auto name = "$d" + std::to_string(i++);
			return coff_writer_.add_symbol(name, 0x1, coff::SymbolTableMsEntryType::IMAGE_SYM_TYPE_NULL, coff::SymbolTableEntryClass::IMAGE_SYM_CLASS_STATIC, value);
		}

		void Generator::add_rel_reloc(u32 vaddr, u32 symndx)
		{
			auto data_reloc = coff::RelocationDirective{ 0 };
			data_reloc.vaddr = vaddr;
			data_reloc.type = static_cast<u16>(coff::RelocationType::IMAGE_REL_AMD64_REL32);
			data_reloc.symndx = symndx;
			coff_writer_.add_relocation(".code", data_reloc);
		}

		find_symbol_result Generator::find_added_symbol(const StringView &name)
		{
			auto it = added_symbols_.find(name.to_string());
			if (it != added_symbols_.end())
			{
				return find_symbol_result(true, it->second.sym_table_index);
			}

			return find_symbol_result(false, 0);
		}

		void Generator::add_addr_entry(const StringView &name, u64 sp_rel_addr)
		{
			addr_tables_.top()->add(name, sp_rel_addr);
		}

		u64 Generator::find_addr_entry(const StringView &name)
		{
			return addr_tables_.top()->find(name).sp_rel_addr;
		}

		void Generator::push_addr_table()
		{
			auto parent = addr_tables_.top().get();
			addr_tables_.push(std::make_unique<AddrTable>(parent));
		}

		void Generator::pop_addr_table()
		{
			addr_tables_.pop();
		}
	}
}
