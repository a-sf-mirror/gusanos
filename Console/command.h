#ifndef COMMAND_H
#define COMMAND_H

#include "consoleitem.h"

#include <string>
#include <boost/function.hpp>

#define VAR_TYPE_INVALID 0
#define VAR_TYPE_INT 1

class Command : public ConsoleItem
{
	public:
	
	typedef boost::function<std::string (std::list<std::string> const&)> CallbackT;
	
	Command(CallbackT const& func);
	Command();
	virtual ~Command();
	
	std::string invoke(std::list<std::string> const& args);
	
	private:
	
	CallbackT m_func;
};

#endif  // _COMMAND_H_
