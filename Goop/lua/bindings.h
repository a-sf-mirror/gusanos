#ifndef LUA_BINDINGS_H
#define LUA_BINDINGS_H

#include "context.h"
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
	
	std::string runLua(int ref, std::list<std::string> const& args);
	void addGUIWndFunctions(LuaContext& context);
	void addGUIListFunctions(LuaContext& context);
	//void pushPlayer(BasePlayer* player);
	//void pushWorm(BaseWorm* worm);
	//void pushViewport(Viewport* worm);
	
	extern int playerIterator;
	extern int playerMetaTable;
	extern int fontMetaTable;
	extern int wormMetaTable;
	extern int baseObjectMetaTable;
	extern int particleMetaTable;
	extern int viewportMetaTable;
	extern std::vector<int> guiWndMetaTable;
}

#endif //LUA_BINDINGS_H
