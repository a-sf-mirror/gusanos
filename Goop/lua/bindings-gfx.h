#ifndef LUA_BINDINGS_GFX_H
#define LUA_BINDINGS_GFX_H

#include "luaapi/context.h"
#include "luaapi/types.h"

#ifndef DEDSERV
#include "../blitters/context.h"
#endif

namespace LuaBindings
{
	void initGfx();

	extern LuaReference viewportMetaTable;
	extern LuaReference bitmapMetaTable;
	extern BlitterContext blitter;
}

#endif //LUA_BINDINGS_GFX_H
