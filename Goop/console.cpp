#include "console.h"
#include "variables.h"

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
		variables[name] = new IntVariable(src, defaultValue);
	}
}

void Console::setVariableValue(string name, string value)
{
	map<string, Variable*>::iterator variable = variables.find(name);
	if (variable != variables.end())
		variable->second->setValue(value);
}

//============================= PRIVATE ======================================



