#include "console.h"


/////////////////////////////// Variable /////////////////////////////////////

//============================= LIFECYCLE ====================================

Variable::Variable()
{
	m_value = NULL;
	m_defaultValue = 0;
	m_type = VAR_TYPE_INVALID;
};

Variable::Variable(int* src, int defaultValue, int type)
{
	m_value = src;
	m_defaultValue = defaultValue;
	m_type = type;
	
	*m_value=defaultValue;
}


/////////////////////////////// Console //////////////////////////////////////

//============================= LIFECYCLE ====================================

Console::Console()
{	
}

Console::~Console()
{
	// Delete the registered variables
	variables.clear();
	
}

//============================= INTERFACE ====================================

void Console::registerIntVariable(int* src, string name, int defaultValue)
{
	if (!name.empty())
	{
		Variable tempVar(src, defaultValue, VAR_TYPE_INT);
		variables[name]=tempVar;
	}
}
