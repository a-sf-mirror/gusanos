#include "variables.h"
#include "text.h"

using namespace std;

///////////////////////////////IntVariable////////////////////////////////

//============================= LIFECYCLE ================================

IntVariable::IntVariable()
{
	m_src=NULL;
	m_defaultValue=0;
	callback = false;
}

IntVariable::~IntVariable()
{
	
}

IntVariable::IntVariable(int* src, string name, int defaultValue, void (*func)( int ))
{
	m_src = src;
	m_name = name;
	m_defaultValue = defaultValue;	
	*m_src = m_defaultValue;
	callback = func;
}

//============================= INTERFACE ================================

string IntVariable::invoke(const std::list<std::string> &args)
{
	if (!args.empty())
	{
		int oldValue = *m_src;
		*m_src=cast<int,string>(*args.begin());
		if ( callback ) callback(oldValue);
		return "";
	}else
	{
		return m_name + " IS \"" + cast<string,int>(*m_src) + '"';
	}
}

///////////////////////////////FloatVariable////////////////////////////////

//============================= LIFECYCLE ================================

FloatVariable::FloatVariable()
{
	m_src=NULL;
	m_defaultValue=0;
}

FloatVariable::~FloatVariable()
{
	
}

FloatVariable::FloatVariable(float* src, string name, float defaultValue, void (*func)( float ) )
{
	m_src = src;
	m_name = name;
	m_defaultValue = defaultValue;	
	*m_src = m_defaultValue;
	callback = func;
}

//============================= INTERFACE ================================

string FloatVariable::invoke(const std::list<std::string> &args)
{
	if (!args.empty())
	{
		float oldValue = *m_src;
		*m_src=cast<float,string>(*args.begin());
		if ( callback ) callback(oldValue);
		return "";
	}else
	{
		return m_name + " IS \"" + cast<string,float>(*m_src) + '"';
	}
}
