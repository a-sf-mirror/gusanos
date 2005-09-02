#ifndef GUSANOS_GLUA_H
#define GUSANOS_GLUA_H

#include <string>
#include <vector>
#include "lua/context.h"

#define EACH_CALLBACK(i_, type_) for(std::vector<int>::iterator i_ = luaCallbacks.type_.begin(); \
			i_ != luaCallbacks.type_.end(); ++i_)

struct LuaCallbacks
{
	void bind(std::string callback, int ref);
	std::vector<int> atGameStart;
	std::vector<int> afterRender;
	std::vector<int> afterUpdate;
	std::vector<int> wormRender;
	std::vector<int> viewportRender;
};

extern LuaContext lua;

extern LuaCallbacks luaCallbacks;

#endif //GUSANOS_GLUA_H
