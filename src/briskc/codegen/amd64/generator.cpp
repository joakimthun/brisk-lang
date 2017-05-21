#include "generator.h"

#include <iostream>

#include "ast/ast.h"

namespace brisk {
	namespace x64 {

		Generator::Generator()
			:
			coff_writer_(coff::MACHINE::IMAGE_FILE_MACHINE_AMD64),
			data_(std::make_unique<ByteBuffer>())
		{
			data_->write((u8)'H');
			data_->write((u8)'e');
			data_->write((u8)'l');
			data_->write((u8)'l');
			data_->write((u8)'o');
			data_->write((u8)' ');
			data_->write((u8)'B');
			data_->write((u8)'r');
			data_->write((u8)'i');
			data_->write((u8)'s');
			data_->write((u8)'k');
			data_->write((u8)'!');
			data_->write((u8)'\0');
		}

		void Generator::visit(Ast &ast)
		{
			std::cout << "Ast" << std::endl;

			for (auto& e : ast.exprs)
				e->accept(*this);
		}

		void Generator::visit(BinExpr &expr)
		{
			auto dest_reg = reg_allocator_.get_free();
			emitter_.emit_mov(dest_reg, 333);
			reg_allocator_.push(dest_reg);

			//expr.left->accept(*this);
			//expr.right->accept(*this);
		}

		void Generator::visit(LiteralExpr &expr)
		{
			std::cout << "LiteralExpr" << std::endl;
		}

		void Generator::visit(IdentifierExpr &expr)
		{
			std::cout << "IdentifierExpr" << std::endl;
		}

		void Generator::visit(AssignExpr &expr)
		{
			std::cout << "AssignExpr" << std::endl;
		}

		void Generator::visit(BlockExpr &expr)
		{
			for (auto& e : expr.exprs)
				e->accept(*this);
		}

		void Generator::visit(FnDeclExpr &expr)
		{
			add_ext_fn_symbol(expr.name);
			expr.body->accept(*this);
		}

		void Generator::visit(RetExpr &expr)
		{
			expr.expr->accept(*this);

			auto src_reg = reg_allocator_.pop();
			emitter_.emit_mov(Register::EAX, src_reg);
			emitter_.emit_ret();
		}

		void Generator::write_to_disk(const std::string &path)
		{
			coff_writer_.add_section(".data", coff::SectionHeaderFlags::STYP_DATA, std::move(data_));
			coff_writer_.add_section(".code", coff::SectionHeaderFlags::STYP_TEXT, emitter_.buffer());
			coff_writer_.write_to_disk(path);
		}

		void Generator::add_ext_fn_symbol(StringView &name)
		{
			coff_writer_.add_symbol(name.to_string(), 0x2, coff::SymbolTableMsEntryType::IMAGE_SYM_DTYPE_FUNCTION, coff::SymbolTableEntryClass::IMAGE_SYM_CLASS_EXTERNAL);
		}

	}
}
