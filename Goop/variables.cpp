#include "variables.h"
#include "text.h"

///////////////////////////////IntVariable////////////////////////////////

//============================= LIFECYCLE ================================

IntVariable::IntVariable()
{
	m_src=NULL;
	m_defaultValue=0;
	m_type=VAR_TYPE_INVALID;
}

IntVariable::~IntVariable()
{
	
}

IntVariable::IntVariable(int* src, int defaultValue)
{
	m_src=src;
	m_defaultValue=defaultValue;
	m_type=VAR_TYPE_INT;
	
	*m_src=m_defaultValue;
}

//============================= INTERFACE ================================

void IntVariable::setValue(string value)
{
	*m_src=cast<int,string>(value);
};

string IntVariable::getValue()
{
	return cast<string,int>(*m_src);
};
