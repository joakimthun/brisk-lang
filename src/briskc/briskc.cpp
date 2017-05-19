#include <iostream>

#include <memory>

#include "brisk_exception.h"
#include "lexing/lexer.h"
#include "parsing/brisk_parser.h"

#include "codegen/coff/coff.h"
#include "codegen/coff/coff_writer.h"
#include "codegen/x86_64/emitter.h"

using namespace brisk;

extern "C" void asm_proc();

// link /DEFAULTLIB:"LIBCMT" /MACHINE:X64 brisk.obj

int main(int argc, char* argv[])
{
	try
	{
		//asm_proc();

		coff::CoffWriter writer(coff::MACHINE::IMAGE_FILE_MACHINE_AMD64);

		/*auto data_content = std::make_unique<ByteBuffer>();
		data_content->write((u8)'H');
		data_content->write((u8)'E');
		data_content->write((u8)'J');
		data_content->write((u8)'!');
		data_content->write((u8)'\0');*/

		auto data_content = std::make_unique<ByteBuffer>();
		data_content->write((u8)'H');
		data_content->write((u8)'e');
		data_content->write((u8)'l');
		data_content->write((u8)'l');
		data_content->write((u8)'o');
		data_content->write((u8)' ');
		data_content->write((u8)'B');
		data_content->write((u8)'r');
		data_content->write((u8)'i');
		data_content->write((u8)'s');
		data_content->write((u8)'k');
		data_content->write((u8)'!');
		data_content->write((u8)'\0');

		writer.add_section(".data", coff::SectionHeaderFlags::STYP_DATA, std::move(data_content));
		
		auto emitter = x64::Emitter();

		emitter.emit_sub64(x64::Register::ESP, 0x28);
		emitter.emit_lea64(x64::Register::ECX, 0);
		emitter.emit_call();
		emitter.emit_xor(x64::Register::EAX, x64::Register::EAX);
		emitter.emit_add64(x64::Register::ESP, 0x28);
		emitter.emit_ret();

		writer.add_section(".code", coff::SectionHeaderFlags::STYP_TEXT, emitter.buffer());

		auto data_reloc = coff::RelocationDirective{ 0 };
		data_reloc.vaddr = 0x7;
		data_reloc.type = 0x4;
		data_reloc.symndx = 1;
		writer.add_relocation(".code", data_reloc);

		auto puts_reloc = coff::RelocationDirective{ 0 };
		puts_reloc.vaddr = 0xc;
		puts_reloc.type = 0x4;
		puts_reloc.symndx = 2;
		writer.add_relocation(".code", puts_reloc);

		writer.add_symbol("main", 0x2, 0x20, 0x2);
		writer.add_symbol("$str", 0x1, 0, 0x03);
		writer.add_symbol("puts", 0x0, 0x20, 0x2);

		writer.write_to_disk("C:/test/brisk.obj");

		//coff::read("C:/test/main.obj");
		coff::read("C:/test/brisk.obj");

		/*auto parser = BriskParser("test_files/test.br");

		auto e = parser.parse_expr();

		auto lexer = Lexer("test_files/test.br");

		auto t = lexer.next();
		while (t.type != TokenType::Eof)
		{
			std::cout << "Value: '" << t.raw_value.to_string();
			std::cout << "' Length: " << t.raw_value.length() << " Start col: " << t.column_start << " End col: " << t.column_end << " Row: " << t.row << std::endl;

			t = lexer.next();
		}*/
	}
	catch (const BriskException& ex)
	{
		auto w = ex.what();
		std::cout << w << std::endl;
	}
}