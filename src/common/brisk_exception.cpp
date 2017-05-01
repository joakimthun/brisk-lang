#include "brisk_exception.h"

namespace brisk {

	BriskException::BriskException() {}
	BriskException::BriskException(const std::string &message) : message_(message) {}

	const char *BriskException::what() const throw()
	{
		return message_.c_str();
	}

}
