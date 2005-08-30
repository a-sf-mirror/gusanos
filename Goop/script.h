#ifndef SCRIPT_H
#define SCRIPT_H

#include "resource_locator.h"

#include <string>
#include <map>


class LuaContext;

class Script
{
public:
	Script()
	: lua(0)
	{
	}
	
	bool pushFunction(std::string const& name);
	
	int createFunctionRef(std::string const& name);

	LuaContext* lua;
	std::string table;
	
private:
	std::map<std::string, int> cachedReferences;
};

extern ResourceLocator<Script> scriptLocator;

#endif //SCRIPT_H
