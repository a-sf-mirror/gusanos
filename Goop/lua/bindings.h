#ifndef LUA_BINDINGS_H
#define LUA_BINDINGS_H

#include "luaapi/context.h"
#include "luaapi/types.h"
#include <vector>
#include <string>
#include <list>

class BaseObject;
class BasePlayer;
class BaseWorm;
class Viewport;

namespace LuaBindings
{
	void init();

	int print(lua_State* state);
	
	std::string runLua(LuaReference ref, std::list<std::string> const& args);
	
}

#endif //LUA_BINDINGS_H
