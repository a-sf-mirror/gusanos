#include "bindings.h"

#include "../gconsole.h"

int LuaBindings::print(lua_State* L)
{
	const char* s = lua_tostring(L, 1);
	console.addLogMsg(s);
	
	return 0;
}

void LuaBindings::init(LuaContext& context)
{
	context.function("print", print);
}