#include <iostream>

#include "brisk_exception.h"
#include "lexing/lexer.h"
#include "parsing/brisk_parser.h"

#include "codegen/coff.h"
#include "codegen/x86_64/emitter.h"

using namespace brisk;

extern "C" void asm_proc();

// link /DEFAULTLIB:"LIBCMT" /MACHINE:X64 brisk.obj

int main(int argc, char* argv[])
{
	try
	{
		//asm_proc();

		auto emitter = x64::Emitter();

		emitter.emit_mov(x64::Register::EBP, 0x0);
		emitter.emit_add(x64::Register::EBP, 0x6);
		emitter.emit_add(x64::Register::EBP, 0x6);
		emitter.emit_sub(x64::Register::EBP, 0x3);
		emitter.emit_mov(x64::Register::EAX, x64::Register::EBP);
		emitter.emit_ret();

		coff::write(emitter.buffer(), "C:/test/brisk.obj");

		//coff::read("C:/test/main.obj");

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