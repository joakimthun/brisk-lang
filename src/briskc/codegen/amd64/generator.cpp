#include "generator.h"

#include <iostream>

#include "ast/ast.h"
#include "type.h"
#include "brisk_exception.h"
#include "token_type.h"

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
			if (primitive_type == nullptr)
				throw BriskException("Generator::visit BinExpr: Only primitive types are currently supported");

			if (primitive_type->is_integral())
			{
				integral_bin_op(expr, primitive_type, left_reg, right_reg);
			}
			else if(primitive_type->is_boolean())
			{
				boolean_bin_op(expr, primitive_type, left_reg, right_reg);
			}
			else
			{
				throw BriskException("Generator::visit BinExpr: Unhandled type: " + expr.type->name());
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

				auto unescaped_string = expr.str_value.to_unescaped_string();
				data_->write(unescaped_string.c_str(), unescaped_string.size());
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
			case TypeID::Bool: {
				emitter_.emit_spd_movzx8(dest_reg, static_cast<u8>(rsp_rel_addr));
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

		void Generator::visit(IfExpr &expr)
		{
			expr.if_condition->accept(*this);

			push_addr_table();
			auto condition_reg = reg_allocator_.pop();

			emitter_.emit_test8(condition_reg, 1);
			const auto rel8_offset = emitter_.emit_je_rel8(0);

			for (auto& arg : expr.if_body)
				arg->accept(*this);

			pop_addr_table();

			const auto current_offset = emitter_.current_buffer_offset() - 1;
			const auto jmp_dist = current_offset - rel8_offset;
			emitter_.emit_rel8_at(jmp_dist, rel8_offset);
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
			case TypeID::Bool: {
				auto addr = stack_allocator_.reserve(1);
				emitter_.emit_spd_mov8(addr, expr.value.b ? 1 : 0);
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

		void Generator::move_to_reg(Register destination, const Type * type, Register source)
		{
			const auto size_in_bytes = type->size();

			switch (size_in_bytes)
			{
			case 1:
				emitter_.emit_mov8(destination, source);
				break;
			case 2:
				emitter_.emit_mov16(destination, source);
				break;
			case 4:
				emitter_.emit_mov32(destination, source);
				break;
			case 8:
				emitter_.emit_mov64(destination, source);
				break;
			default:
				throw BriskException("Generator::move_to_reg: Invalid size");
			}
		}

		void Generator::integral_bin_op(BinExpr &expr, const PrimitiveType *primitive_type, Register left_reg, Register right_reg)
		{
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
					emitter_.emit_sub64(right_reg, left_reg);
				}
				else
				{
					emitter_.emit_sub32(right_reg, left_reg);
				}

				move_to_reg(right_reg, expr.type, left_reg);

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
			case TokenType::Slash: 
			case TokenType::Modulus : {
				move_to_reg(Register::RAX, expr.type, left_reg);

				// Remainder stored in EDX/RDX
				emitter_.emit_xor64(Register::RDX, Register::RDX);

				if (primitive_type->size() == 8)
				{
					emitter_.emit_idiv64(right_reg);
				}
				else
				{
					emitter_.emit_idiv32(right_reg);
				}

				if (expr.op == TokenType::Slash)
				{
					move_to_reg(right_reg, expr.type, Register::RAX);
				}
				else
				{
					move_to_reg(right_reg, expr.type, Register::RDX);
				}

				break;
			}
			default:
				throw BriskException("Generator::integral_bin_op: Unhandled operator " + token_type_name(expr.op));
			}
		}

		void Generator::boolean_bin_op(BinExpr &expr, const PrimitiveType *primitive_type, Register left_reg, Register right_reg)
		{
			switch (expr.op)
			{
			case TokenType::LogOr: {
				// left == true
				emitter_.emit_test8(left_reg, 1);
				// jump to true branch
				emitter_.emit_jne_rel8(10);

				// right == true
				emitter_.emit_test8(right_reg, 1);
				// jump to true branch
				emitter_.emit_jne_rel8(5);

				// false branch
				emitter_.emit_mov8(right_reg, 0);
				// jump past true branch
				emitter_.emit_jmp_rel8(3);

				// true branch
				emitter_.emit_mov8(right_reg, 1);

				break;
			}
			case TokenType::LogAnd: {
				// left == true
				emitter_.emit_test8(left_reg, 1);
				// jump to false branch if false
				emitter_.emit_je_rel8(10);

				// right == true
				emitter_.emit_test8(right_reg, 1);
				// jump to false branch if false
				emitter_.emit_je_rel8(5);

				// true branch
				emitter_.emit_mov8(right_reg, 1);
				// jump past false branch
				emitter_.emit_jmp_rel8(3);

				// false branch
				emitter_.emit_mov8(right_reg, 0);

				break;
			}
			case TokenType::DoubleEquals:
			case TokenType::NotEqual: {
				const auto use_64 = expr.left->type->size() == 8 || expr.right->type->size() == 8;
				if (use_64)
				{
					emitter_.emit_cmp64(left_reg, right_reg);
				}
				else
				{
					emitter_.emit_cmp32(left_reg, right_reg);
				}

				if (expr.op == TokenType::DoubleEquals)
				{
					emitter_.emit_jne_rel8(5);
				}
				else 
				{
					emitter_.emit_je_rel8(5);
				}

				// true branch
				emitter_.emit_mov8(right_reg, 1);
				// jump past false branch
				emitter_.emit_jmp_rel8(3);

				// false branch
				emitter_.emit_mov8(right_reg, 0);

				break;
			}
			default:
				throw BriskException("Generator::boolean_bin_op: Unhandled operator " + token_type_name(expr.op));
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
