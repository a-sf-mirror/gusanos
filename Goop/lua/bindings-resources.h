#ifndef LUA_BINDINGS_RESOURCES_H
#define LUA_BINDINGS_RESOURCES_H

#include "types.h"

namespace LuaBindings
{
	void initResources();
	
	extern LuaReference fontMetaTable;
	extern LuaReference partTypeMetaTable;
}

#endif //LUA_BINDINGS_RESOURCES_H
