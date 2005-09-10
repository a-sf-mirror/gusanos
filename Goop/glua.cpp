#include "glua.h"

LuaContext lua;

LuaCallbacks luaCallbacks;

void LuaCallbacks::bind(std::string callback, int ref)
{
	if(callback == "afterRender")
		afterRender.push_back(ref);
	else if(callback == "afterUpdate")
		afterUpdate.push_back(ref);
	else if(callback == "wormRender")
		wormRender.push_back(ref);
	else if(callback == "viewportRender")
		viewportRender.push_back(ref);
}
