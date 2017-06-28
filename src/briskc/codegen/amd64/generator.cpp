#include "generator.h"

#include <iostream>

#include "ast/ast.h"
#include "type.h"
#include "brisk_exception.h"

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
			for (auto& e : ast.exprs)
				e->accept(*this);
		}

		void Generator::visit(BinExpr &expr)
		{
			expr.left->accept(*this);
			auto left_reg = reg_allocator_.pop();
			expr.right->accept(*this);
			auto right_reg = reg_allocator_.pop();

			const auto primitive_type = expr.type->as_const<PrimitiveType>();
			if (primitive_type == nullptr || !primitive_type->is_integral())
				throw BriskException("Generator::visit BinExpr: Unhandled type: " + expr.type->name());

			switch (expr.op)
			{
			case TokenType::Plus: {
				if (primitive_type->size() == 8)
				{
					emitter_.emit_add64(left_reg, right_reg);
				}
				else
				{
					emitter_.emit_add32(left_reg, right_reg);
				}

				break;
			}
			case TokenType::Minus: {
				if (primitive_type->size() == 8)
				{
					emitter_.emit_sub64(left_reg, right_reg);
				}
				else
				{
					emitter_.emit_sub32(left_reg, right_reg);
				}

				break;
			}
			case TokenType::Star: {
				if (primitive_type->size() == 8)
				{
					emitter_.emit_imul64(left_reg, right_reg);
				}
				else
				{
					emitter_.emit_imul32(left_reg, right_reg);
				}

				break;
			}
			case TokenType::Slash: {
				throw BriskException("Generator::visit BinExpr: Unhandled operator slash");
			}
			default:
				break;
			}

			reg_allocator_.push(right_reg);
			reg_allocator_.free(left_reg);
		}

		void Generator::visit(LiteralExpr &expr)
		{
			if (expr.str_literal)
			{
				auto reg = reg_allocator_.get_free();
				emitter_.emit_lea64(reg, 0);
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
				emitter_.emit_mov32(dest_reg, expr.value.i32);
				reg_allocator_.push(dest_reg);
			}
		}

		void Generator::visit(IdentifierExpr &expr)
		{
			auto rsp_rel_addr = find_addr_entry(expr.name);
			auto dest_reg = reg_allocator_.get_free();

			switch (expr.type->id())
			{
			case TypeID::U8: {
				// Temporary work-around for string literals
				if (expr.type->is_ptr())
				{
					emitter_.emit_spd_mov64(dest_reg, rsp_rel_addr);
				}
				else
				{
					emitter_.emit_spd_movzx8(dest_reg, static_cast<u8>(rsp_rel_addr));
				}
				break;

			}
			case TypeID::I8: {
				emitter_.emit_spd_movsx8(dest_reg, static_cast<u8>(rsp_rel_addr));
				break;
			}
			case TypeID::U16: {
				emitter_.emit_spd_movzx16(dest_reg, static_cast<u8>(rsp_rel_addr));
				break;
			}
			case TypeID::I16: {
				emitter_.emit_spd_movsx16(dest_reg, static_cast<u8>(rsp_rel_addr));
				break;
			}
			case TypeID::U32: 
			case TypeID::I32: {
				emitter_.emit_spd_mov32(dest_reg, static_cast<u8>(rsp_rel_addr));
				break;
			}
			case TypeID::U64:
			case TypeID::I64: {
				emitter_.emit_spd_mov64(dest_reg, static_cast<u8>(rsp_rel_addr));
				break;
			}
			case TypeID::Float:
				throw BriskException("Generator::visit IdentifierExpr: Unhandled TypeID float");
			case TypeID::Double:
				throw BriskException("Generator::visit IdentifierExpr: Unhandled TypeID double");
			default:
				throw BriskException("Generator::visit IdentifierExpr: Unhandled TypeID");
			}

			reg_allocator_.push(dest_reg);
		}

		void Generator::visit(AssignExpr &expr)
		{
			std::cout << "AssignExpr" << std::endl;
		}

		void Generator::visit(FnDeclExpr &expr)
		{
			if (expr.ext)
				return;

			push_addr_table();
			add_fn_symbol(expr.name, emitter_.current_buffer_offset());
			stack_allocator_.reserve(expr);
			stack_allocator_.reset();
			
			store_fn_args(expr);

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
			emitter_.emit_mov32(Register::EAX, src_reg);
		}

		void Generator::visit(VarDeclExpr &expr)
		{
			auto literal_expr = expr.expr->as<LiteralExpr>();

			if (literal_expr != nullptr && !literal_expr->str_literal)
			{
				add_addr_entry(expr.name, store_literal_to_mem(*literal_expr));
			}
			else
			{
				expr.expr->accept(*this);
				auto value_reg = reg_allocator_.pop();
				auto sp_rel_addr = stack_allocator_.reserve(expr.type->size());
				move_to_reg(sp_rel_addr, expr.type, value_reg);

				add_addr_entry(expr.name, sp_rel_addr);
				reg_allocator_.free(value_reg);
			}
		}

		void Generator::visit(FnCallExpr &expr)
		{
			// Integer arguments are passed in registers RCX, RDX, R8, and R9. Floating point arguments are passed in XMM0L, XMM1L, XMM2L, and XMM3L.
			// In addition to these registers, RAX, R10, R11, XMM4, and XMM5 are considered volatile. 
			reg_allocator_.prepare_call();

			for (auto& arg : expr.args)
				arg->accept(*this);

			reg_allocator_.end_call_prepare();

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

		void Generator::store_fn_args(FnDeclExpr &expr)
		{
			if (expr.args.size() == 0)
				return;

			// Base args RSP offset
			const u8 base = 0x8;
			
			for (i64 i = expr.args.size() - 1; i >= 0; i--)
			{
				const auto& arg = expr.args[i];
				u8 sp_rel_addr = base;

				if (i == 0)
				{
					move_to_reg(sp_rel_addr, arg->type, Register::ECX);
				}
				else if(i == 1)
				{
					sp_rel_addr *= 2;
					move_to_reg(sp_rel_addr, arg->type, Register::EDX);
				}
				else if (i == 2)
				{
					sp_rel_addr *= 3;
					move_to_reg(sp_rel_addr, arg->type, Register::R8);
				}
				else if (i == 3)
				{
					sp_rel_addr *= 4;
					move_to_reg(sp_rel_addr, arg->type, Register::R9);
				}
				else
				{
					throw BriskException("Generator::store_fn_args: Need to handle stack args...");
				}

				add_addr_entry(arg->name, sp_rel_addr);
			}
		}

		sp_rel_addr Generator::store_literal_to_mem(LiteralExpr &expr)
		{
			switch (expr.type->id())
			{
			case TypeID::U8: {
				auto addr = stack_allocator_.reserve(1);
				emitter_.emit_spd_mov8(addr, expr.value.u8);
				return addr;
			}
			case TypeID::I8: {
				auto addr = stack_allocator_.reserve(1);
				emitter_.emit_spd_mov8(addr, static_cast<u8>(expr.value.i8));
				return addr;
			}
			case TypeID::U16: {
				auto addr = stack_allocator_.reserve(2);
				emitter_.emit_spd_mov16(addr, expr.value.u16);
				return addr;
			}
			case TypeID::I16: {
				auto addr = stack_allocator_.reserve(2);
				emitter_.emit_spd_mov16(addr, static_cast<u16>(expr.value.i16));
				return addr;
			}
			case TypeID::U32: {
				auto addr = stack_allocator_.reserve(4);
				emitter_.emit_spd_mov32(addr, expr.value.u32);
				return addr;
			}
			case TypeID::I32: {
				auto addr = stack_allocator_.reserve(4);
				emitter_.emit_spd_mov32(addr, static_cast<u32>(expr.value.i32));
				return addr;
			}
			case brisk::TypeID::U64: {
				// There is no MOV r/m64, imm64 instruction, it seems...
				auto reg = reg_allocator_.get_free();
				emitter_.emit_mov64(reg, expr.value.u64);
				auto addr = stack_allocator_.reserve(8);
				emitter_.emit_spd_mov64(addr, reg);
				reg_allocator_.free(reg);
				return addr;
			}
			case TypeID::I64: {
				// There is no MOV r/m64, imm64 instruction, it seems...
				auto reg = reg_allocator_.get_free();
				emitter_.emit_mov64(reg, static_cast<u64>(expr.value.i64));
				auto addr = stack_allocator_.reserve(8);
				emitter_.emit_spd_mov64(addr, reg);
				reg_allocator_.free(reg);
				return addr;
			}
			case TypeID::Float:
				throw BriskException("Generator::store_literal_to_mem: Unhandled TypeID float");
			case TypeID::Double:
				throw BriskException("Generator::store_literal_to_mem: Unhandled TypeID double");
			default:
				throw BriskException("Generator::store_literal_to_mem: Unhandled TypeID");
			}
		}

		void Generator::move_to_reg(u8 sp_rel_addr, const Type *type, Register source)
		{
			const auto size_in_bytes = type->size();

			switch (size_in_bytes)
			{
			case 1:
				emitter_.emit_spd_mov8(sp_rel_addr, source);
				break;
			case 2:
				emitter_.emit_spd_mov16(sp_rel_addr, source);
				break;
			case 4:
				emitter_.emit_spd_mov32(sp_rel_addr, source);
				break;
			case 8:
				emitter_.emit_spd_mov64(sp_rel_addr, source);
				break;
			default:
				throw BriskException("Generator::move_to_reg: Invalid size");
			}
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
