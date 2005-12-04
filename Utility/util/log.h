#ifndef UTILITY_LOG_H
#define UTILITY_LOG_H

#include <iostream>
#include <string>

struct Location
{
	Location()
	: file(0)
	{
	}
	
	Location(Location const& l)
	: file(l.file), line(l.line)
	{
	}
	
	Location(std::string const& file_, int line_)
	: file(&file_), line(line_)
	{
	}
	
	// Compiler generated op=
		
	void print(std::string const& msg) const;
	
private:
	std::string const* file;
	int line;
};

#define DLOG(x_) (std::cout << x_ << std::endl)
#define DLOGL(l_, x_) l_.print(x_)

#endif //UTILITY_LOG_H
