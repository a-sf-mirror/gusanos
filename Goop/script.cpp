#include "script.h"

#include <iostream>
#include "lua/context.h"
using std::string;
using std::cerr;
using std::endl;

ResourceLocator<Script> scriptLocator;

bool Script::pushFunction(std::string const& name)
{
	if(!lua)
		return false;
	
	lua_pushstring(*lua, table.c_str());
	lua_rawget(*lua, LUA_GLOBALSINDEX);
	if(lua_isnil(*lua, -1))
	{
		cerr << "Table " << table << " not found!" << endl;
		return false;
	}
	lua_pushstring(*lua, name.c_str());
	lua_rawget(*lua, -2);
	lua_replace(*lua, -2);
	
	return true;
}

int Script::createFunctionRef(std::string const& name)
{
	if(!lua)
		return 0;
		
	std::map<std::string, int>::const_iterator i = cachedReferences.find(name);
	
	if(i != cachedReferences.end())
		return i->second; // Reference cached
		
	if(!pushFunction(name))
		return 0;
		
	int ref = lua->createReference();
	cachedReferences[name] = ref; // Cache reference
	return ref;
}
