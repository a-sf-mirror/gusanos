#ifndef alias_h
#define alias_h

#include "console.h"
#include "consoleItem.h"

#include <string>

#define VAR_TYPE_INVALID 0
#define VAR_TYPE_INT 1

class Alias : public ConsoleItem
{
	public:
	
	Alias(Console *parent, const std::string &name, const std::string &action);
	
	Alias();
	~Alias();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:
	
	Console *m_parent;
	std::string m_name;
	std::string m_action;
};

#endif  // _alias_h_
