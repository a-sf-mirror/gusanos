#ifndef command_h
#define command_h

#include "consoleItem.h"

#include <string>

#define VAR_TYPE_INVALID 0
#define VAR_TYPE_INT 1

class Command : public ConsoleItem
{
	public:
	
	Command(std::string (*func)(const std::list<std::string>&));
	Command();
	~Command();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:
	
	std::string (*m_func)(const std::list<std::string>&);
};

#endif  // _command_h_
