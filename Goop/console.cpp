#include "console.h"
#include "variables.h"
#include <allegro.h>

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
	int leftIndex = 0;
	int rightIndex = 0;
	
	if (!text.empty())
	{
		rightIndex = text.find_first_of(';');
		while (rightIndex != string::npos)
		{
			parse(text.substr(leftIndex, rightIndex - leftIndex));
			leftIndex=rightIndex+1;
			rightIndex = text.find_first_of(';',leftIndex);
		}
		parse(text.substr(leftIndex));
	}
}

void Console::parse(const string &text)
{
	int leftIndex = 0;
	int rightIndex = 0;
	
	string itemName;
	string arguments;
	
	if (!text.empty())
	{
		leftIndex = text.find_first_not_of(' ');
		if (leftIndex != string::npos)
		{
			rightIndex = text.find_first_of(' ', leftIndex);
			if (rightIndex != string::npos)
			{
				itemName = text.substr(leftIndex, rightIndex - leftIndex);
			}else
				itemName = text.substr(leftIndex);
		}
		leftIndex = text.find_first_not_of(' ',rightIndex);
		if (leftIndex != string::npos)
		{
			rightIndex = text.find_first_of(' ', leftIndex);
			if (rightIndex != string::npos)
			{
				arguments = text.substr(leftIndex, rightIndex - leftIndex);
			}else
				arguments = text.substr(leftIndex);
		}
		
		if (!itemName.empty())
		{
			allegro_message(itemName.c_str());
			if (!arguments.empty())
			{
				allegro_message(arguments.c_str());
			}
		}
	};
}

//============================= PRIVATE ======================================






