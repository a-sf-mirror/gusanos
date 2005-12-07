#ifndef LUA_BINDINGS_RESOURCES_H
#define LUA_BINDINGS_RESOURCES_H

#include "luaapi/types.h"

namespace LuaBindings
{
	void initResources();
	
	extern LuaReference fontMetaTable;
	extern LuaReference partTypeMetaTable;
	extern LuaReference weaponTypeMetaTable;
	extern LuaReference spritesMetaTable;
}

#endif //LUA_BINDINGS_RESOURCES_H
