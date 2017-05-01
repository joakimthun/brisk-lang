#include <iostream>

#include "exceptions/brisk_exception.h"
#include "file.h"

using namespace brisk;

int main(int argc, char* argv[])
{
	try
	{
		const auto f = open_file("test_files/test.br");

		for (auto i = 0; i < f->length; i++)
			std::cout << f->content[i];

		std::cout << std::endl;
	}
	catch (const BriskException& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}