#ifndef LUA_BINDINGS_H
#define LUA_BINDINGS_H

#include "context.h"
#include <vector>

class LuaBindings
{
public:
	static void init(LuaContext& context);

	static int print(lua_State* state);
	
	static void addGUIWndFunctions(LuaContext& context);
	static void addGUIListFunctions(LuaContext& context);
	
	static int playerIterator;
	static int playerMetaTable;
	static std::vector<int> guiWndMetaTable;
};

#endif //LUA_BINDINGS_H
