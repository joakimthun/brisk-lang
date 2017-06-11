#include <iostream>
#include <memory>
#include <cstdlib>

#include "brisk_exception.h"
#include "lexing/lexer.h"
#include "parsing/brisk_parser.h"
#include "types/type_table.h"
#include "types/type_checker.h"

#include "codegen/coff/coff.h"
#include "codegen/coff/coff_writer.h"
#include "codegen/amd64/emitter.h"
#include "codegen/amd64/generator.h"

using namespace brisk;

#define DISASM 1

extern "C" void asm_proc();

int main(int argc, char* argv[])
{
	try
	{
		auto type_table = TypeTable();

		std::cout << "Compiling..." << std::endl;
		auto parser = BriskParser("test_files/test_utf8.br", type_table);
		auto ast = parser.parse();

		auto type_checker = TypeChecker();
		type_checker.visit(*ast);

		auto x64gen = x64::Generator();
		x64gen.visit(*ast);

		x64gen.write_to_disk("C:/test/brisk.obj");

		std::cout << "Linking..." << std::endl;
		std::system("link /DEFAULTLIB:\"LIBCMT\" /MACHINE:X64 /OUT:c:/test/brisk.exe C:/test/brisk.obj");

#if DISASM
		std::cout << "Disasm..." << std::endl;
		std::system("dumpbin /disasm C:/test/brisk.obj");
		std::cout << std::endl;
#endif

		std::cout << "Running..." << std::endl;
		std::system("C:/test/brisk.exe");
	}
	catch (const BriskException& ex)
	{
		auto w = ex.what();
		std::cout << w << std::endl;
	}
}