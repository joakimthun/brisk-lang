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

		
		auto emitter = x64::Emitter();

		emitter.emit_mov64(x64::Register::EBP, 0x0);
		//emitter.emit_add(x64::Register::EBP, 0x6);
		//emitter.emit_add(x64::Register::EBP, 0x6);
		//emitter.emit_sub(x64::Register::EBP, 0x3);
		//emitter.emit_mov(x64::Register::EAX, x64::Register::EBP);
		emitter.emit_sub64(x64::Register::ESP, 0x28);
		emitter.emit_call();
		emitter.emit_add64(x64::Register::ESP, 0x28);
		emitter.emit_ret();

		writer.add_section(".code", coff::SectionHeaderFlags::STYP_TEXT, emitter.buffer());

		auto data = std::make_unique<ByteBuffer>();
		data->write((u8)'H');
		data->write((u8)'E');
		data->write((u8)'J');
		data->write((u8)'!');
		data->write((u8)'\0');

		writer.add_section(".data", coff::SectionHeaderFlags::STYP_DATA, std::move(data));

		writer.add_symbol("main", 1, 32, 2);

		writer.write_to_disk("C:/test/brisk.obj");

		coff::read("C:/test/main.obj");

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