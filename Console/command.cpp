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

Command::Command(CallbackT const& func)
: m_func(func)
{

}

//============================= INTERFACE ================================

string Command::invoke(std::list<std::string> const& args)
{
	return m_func(args);
}
