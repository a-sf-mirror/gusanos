#ifndef variables_h
#define variables_h

#include "consoleitem.h"

#include <string>
#include <allegro.h>

#define VAR_TYPE_INVALID 0
#define VAR_TYPE_INT 1

class IntVariable : public ConsoleItem
{
	public:
	
	IntVariable(int* src, std::string name, int defaultValue);
	IntVariable();
	~IntVariable();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:
	
	std::string m_name;
	int* m_src;
	int m_defaultValue;
};

#endif  // _variables_h_
