#ifndef LUA_BINDINGS_H
#define LUA_BINDINGS_H

#include "context.h"

class LuaBindings
{
public:
	static void init(LuaContext& context);

private:
	static int print(lua_State* state);
};

#endif //LUA_BINDINGS_H
