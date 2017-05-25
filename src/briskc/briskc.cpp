#include <iostream>

#include <memory>

#include "brisk_exception.h"
#include "lexing/lexer.h"
#include "parsing/brisk_parser.h"

#include "codegen/coff/coff.h"
#include "codegen/coff/coff_writer.h"
#include "codegen/amd64/emitter.h"
#include "codegen/amd64/generator.h"

using namespace brisk;

extern "C" void asm_proc();

// link /DEFAULTLIB:"LIBCMT" /MACHINE:X64 brisk.obj

int main(int argc, char* argv[])
{
	try
	{
		//asm_proc();

		coff::read("C:/test/main.obj");
		//coff::read("C:/test/brisk2.obj");

		auto lexer = Lexer("test_files/test2.br");

		auto t = lexer.next();
		while (t.type != TokenType::Eof)
		{
			std::cout << "Value: '" << t.raw_value.to_string();
			std::cout << "' Length: " << t.raw_value.length() << " Start col: " << t.column_start << " End col: " << t.column_end << " Row: " << t.row << std::endl;

			t = lexer.next();
		}

		auto parser = BriskParser("test_files/test2.br");
		auto ast = parser.parse();

		auto x64gen = x64::Generator();
		x64gen.visit(*ast);

		x64gen.write_to_disk("C:/test/brisk2.obj");
	}
	catch (const BriskException& ex)
	{
		auto w = ex.what();
		std::cout << w << std::endl;
	}
}