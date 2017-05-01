#include <iostream>

#include "brisk_exception.h"
#include "lexing/lexer.h"

using namespace brisk;

int main(int argc, char* argv[])
{
	try
	{
		auto lexer = Lexer("test_files/test.br");

		auto t = lexer.next();
		while (t.type != TokenType::Eof)
		{
			std::cout << "Value: '";
			for (auto i = 0; i < t.length; i++)
				std::cout << t.raw_value[i];

			std::cout << "' Length: " << t.length << " Start col: " << t.column_start << " End col: " << t.column_end << " Row: " << t.row << std::endl;

			t = lexer.next();
		}
	}
	catch (const BriskException& ex)
	{
		auto w = ex.what();
		std::cout << w << std::endl;
	}
}