#include "variables.h"
#include "text.h"

using namespace std;

///////////////////////////////IntVariable////////////////////////////////

//============================= LIFECYCLE ================================

IntVariable::IntVariable()
{
	m_src=NULL;
	m_defaultValue=0;
}

IntVariable::~IntVariable()
{
	
}

IntVariable::IntVariable(int* src, string name, int defaultValue)
{
	m_src = src;
	m_name = name;
	m_defaultValue = defaultValue;	
	*m_src = m_defaultValue;
}

//============================= INTERFACE ================================

string IntVariable::invoke(const std::list<std::string> &args)
{
	if (!args.empty())
	{
		*m_src=cast<int,string>(*args.begin());
		return "";
	}else
	{
		return m_name + " is \"" + cast<string,int>(*m_src) + '"';
	}
}
