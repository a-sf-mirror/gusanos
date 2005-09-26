#ifndef LUA_BINDINGS_GAME_H
#define LUA_BINDINGS_GAME_H

#include "context.h"
#include "types.h"

namespace LuaBindings
{
	void initGame();
	
	extern LuaReference playerIterator;
	extern LuaReference playerMetaTable;
}

#endif //LUA_BINDINGS_GAME_H
