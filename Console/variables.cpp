#include "variables.h"
#include "text.h"

using namespace std;


/*
///////////////////////////////IntVariable////////////////////////////////

//============================= LIFECYCLE ================================

IntVariable::IntVariable()
: m_src(NULL), m_defaultValue(0), callback(0)
{

}

IntVariable::~IntVariable()
{
	
}

IntVariable::IntVariable(int* src, string name, int defaultValue, void (*func)( int ))
: Variable(name), m_src(src), m_defaultValue(defaultValue), callback(func)
{
	*m_src = m_defaultValue;
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
		return m_name + " IS \"" + cast<string>(*m_src) + '"';
	}
}

///////////////////////////////FloatVariable////////////////////////////////

//============================= LIFECYCLE ================================

FloatVariable::FloatVariable()
: m_src(NULL), m_defaultValue(0.f), callback(0)
{

}

FloatVariable::~FloatVariable()
{
	
}

FloatVariable::FloatVariable(float* src, string name, float defaultValue, void (*func)( float ) )
: Variable(name), m_src(src), m_defaultValue(defaultValue), callback(func)
{
	*m_src = m_defaultValue;
}

//============================= INTERFACE ================================

string FloatVariable::invoke(const std::list<std::string> &args)
{
	if (!args.empty())
	{
		float oldValue = *m_src;
		*m_src = cast<float>(*args.begin());
		if ( callback ) callback(oldValue);

		return "";
	}else
	{
		return m_name + " IS \"" + cast<string>(*m_src) + '"';
	}
}
*/

///////////////////////////////EnumVariable////////////////////////////////

//============================= LIFECYCLE ================================

EnumVariable::EnumVariable()
: m_src(NULL), m_defaultValue(0)
{

}

EnumVariable::~EnumVariable()
{
	
}

EnumVariable::EnumVariable(std::string name, int* src, int defaultValue, MapType const& mapping, void (*func)( int ))
: Variable(name), m_src(src), m_defaultValue(defaultValue), m_mapping(mapping)
{
	*m_src = m_defaultValue;

	for(MapType::iterator i = m_mapping.begin(); i != m_mapping.end(); ++i)
	{
		m_reverseMapping[i->second] = i->first;
	}
}

//============================= INTERFACE ================================

string EnumVariable::invoke(const std::list<std::string> &args)
{
	if (!args.empty())
	{
		MapType::iterator v = m_mapping.find(*args.begin());
		if(v == m_mapping.end())
		{
			string help = "INVALID VALUE, POSSIBLE VALUES: ";
			
			for(MapType::iterator i = m_mapping.begin(); i != m_mapping.end(); ++i)
			{
				help += i->first + ' ';
			}
			
			return help;
		}
		
		*m_src = v->second;
		return "";
	}else
	{
		ReverseMapType::iterator v = m_reverseMapping.find(*m_src);
		
		if(v == m_reverseMapping.end())
			return m_name + " HAS ILLEGAL NUMERIC VALUE " + cast<string>(*m_src);
		
		//return m_name + " IS \"" + v->second + '"';
		return v->second;
	}
}
