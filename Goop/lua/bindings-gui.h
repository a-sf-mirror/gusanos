#ifndef LUA_BINDINGS_GUI_H
#define LUA_BINDINGS_GUI_H

#include "context.h"
#include "types.h"
#include <vector>

namespace LuaBindings
{
	void initGUI();

	extern std::vector<LuaReference> guiWndMetaTable;
}

#endif //LUA_BINDINGS_GUI_H
