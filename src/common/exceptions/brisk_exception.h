#pragma once

#include <exception>
#include <string>

namespace brisk {

	class BriskException : public std::exception
	{
	public:
		BriskException(const std::string& message);

		virtual const char* what() const throw();

	protected:
		std::string message_;
	};
}
