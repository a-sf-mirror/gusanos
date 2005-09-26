#ifndef LUA_BINDINGS_GFX_H
#define LUA_BINDINGS_GFX_H

#include "context.h"
#include "types.h"

namespace LuaBindings
{
	void initGfx();

	extern LuaReference viewportMetaTable;
}

#endif //LUA_BINDINGS_GFX_H
