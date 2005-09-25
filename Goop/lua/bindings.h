#ifndef LUA_BINDINGS_H
#define LUA_BINDINGS_H

#include "context.h"
#include "types.h"
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
	void addGUIWndFunctions(LuaContext& context);
	void addGUIListFunctions(LuaContext& context);
	//void pushPlayer(BasePlayer* player);
	//void pushWorm(BaseWorm* worm);
	//void pushViewport(Viewport* worm);
	
	extern LuaReference playerIterator;
	extern LuaReference playerMetaTable;
	extern LuaReference fontMetaTable;
	extern LuaReference wormMetaTable;
	extern LuaReference baseObjectMetaTable;
	extern LuaReference particleMetaTable;
	extern LuaReference viewportMetaTable;
	extern std::vector<LuaReference> guiWndMetaTable;
}

#endif //LUA_BINDINGS_H
