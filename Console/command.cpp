#include "command.h"

using namespace std;

///////////////////////////////IntVariable////////////////////////////////

//============================= LIFECYCLE ================================

Command::Command()
{
	m_func=NULL;
}

Command::~Command()
{
}

Command::Command(std::string (*func)(const std::list<std::string>&))
{
	m_func=func;
}

//============================= INTERFACE ================================

string Command::invoke(const std::list<std::string> &args)
{
	return m_func(args);
}
