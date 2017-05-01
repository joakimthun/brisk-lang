#pragma once

#include <exception>
#include <string>

namespace brisk {

	class BriskException : public std::exception
	{
	public:
		BriskException();
		BriskException(const std::string& message);

		virtual const char* what() const override;

	protected:
		std::string message_;
	};
}
