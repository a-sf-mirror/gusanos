#include "alias.h"
#include "omfgutil_text.h"

using namespace std;

///////////////////////////////IntVariable////////////////////////////////

//============================= LIFECYCLE ================================

Alias::Alias()
{
	m_parent=NULL;
}

Alias::~Alias()
{
	
}

Alias::Alias(Console *parent, const std::string &name, const std::string &action) :
ConsoleItem(false), m_name(name), m_action(action), m_parent(parent)
{

}

//============================= INTERFACE ================================

string Alias::invoke(const std::list<std::string> &args)
{
	if (!m_action.empty())
	{
		m_parent->parseLine(m_action);
	}
	return "";
}

