#ifndef LUA_BINDINGS_GFX_H
#define LUA_BINDINGS_GFX_H

#include "luaapi/context.h"
#include "luaapi/types.h"

namespace LuaBindings
{
	void initGfx();

	extern LuaReference viewportMetaTable;
	extern LuaReference bitmapMetaTable;
}

#endif //LUA_BINDINGS_GFX_H
