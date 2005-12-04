#include "log.h"

void Location::print(std::string const& msg) const
{
	if(file)
		std::cerr << *file << ':' << line << ": " << msg << '\n';
}
