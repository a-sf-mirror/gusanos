#include "console.h"

#include "variables.h"
#include "text.h"

#include <allegro.h>

using namespace std;

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

void Console::registerIntVariable(int* src, const string &name, int defaultValue)
{
	if (!name.empty())
	{
		variables[name] = new IntVariable(src, defaultValue);
	}
}

void Console::setVariableValue(const string &name, const string &value)
{
	map<string, Variable*>::iterator variable = variables.find(name);
	if (variable != variables.end())
		variable->second->setValue(value);
}

void Console::parseLine(const string &text)
{
	string textToParse;
	string textInQueue = text;
	
	if (!text.empty())
	{
		separate_str_by(';', textInQueue, textToParse, textInQueue);
		while (!textInQueue.empty())
		{
			parse(textToParse);
			separate_str_by(';', textInQueue, textToParse, textInQueue);
		}
		parse(textToParse);
	}
}

void Console::parse(const string &text)
{
	string itemName;
	string arguments;
	
	separate_str_by(' ',text,itemName,arguments);
	
	setVariableValue(itemName, arguments);
}

//============================= PRIVATE ======================================






