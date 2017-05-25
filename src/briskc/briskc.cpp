#include <iostream>
#include <memory>
#include <cstdlib>

#include "brisk_exception.h"
#include "lexing/lexer.h"
#include "parsing/brisk_parser.h"

#include "codegen/coff/coff.h"
#include "codegen/coff/coff_writer.h"
#include "codegen/amd64/emitter.h"
#include "codegen/amd64/generator.h"

using namespace brisk;

extern "C" void asm_proc();

int main(int argc, char* argv[])
{
	try
	{
		//asm_proc();

		//coff::read("C:/test/main.obj");
		//coff::read("C:/test/brisk.obj");

		auto lexer = Lexer("test_files/test2.br");

		auto t = lexer.next();
		while (t.type != TokenType::Eof)
		{
			std::cout << "Value: '" << t.raw_value.to_string();
			std::cout << "' Length: " << t.raw_value.length() << " Start col: " << t.column_start << " End col: " << t.column_end << " Row: " << t.row << std::endl;

			t = lexer.next();
		}

		std::cout << "Compiling..." << std::endl;
		auto parser = BriskParser("test_files/test2.br");
		auto ast = parser.parse();

		auto x64gen = x64::Generator();
		x64gen.visit(*ast);

		x64gen.write_to_disk("C:/test/brisk.obj");

		std::cout << std::endl;
		std::cout << std::endl;

		std::cout << "Linking..." << std::endl;
		std::system("link /DEFAULTLIB:\"LIBCMT\" /MACHINE:X64 /OUT:c:/test/brisk.exe C:/test/brisk.obj");

		std::cout << "Running..." << std::endl;
		std::system("C:/test/brisk.exe");
	}
	catch (const BriskException& ex)
	{
		auto w = ex.what();
		std::cout << w << std::endl;
	}
}