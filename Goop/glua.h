#ifndef GUSANOS_GLUA_H
#define GUSANOS_GLUA_H

#include <string>
#include <vector>
#include "lua/context.h"
#include "lua/types.h"

#define EACH_CALLBACK(i_, type_) for(std::vector<LuaReference>::iterator i_ = luaCallbacks.type_.begin(); \
			i_ != luaCallbacks.type_.end(); ++i_)

struct LuaCallbacks
{
	void bind(std::string callback, LuaReference ref);
	std::vector<LuaReference> atGameStart;
	std::vector<LuaReference> afterRender;
	std::vector<LuaReference> afterUpdate;
	std::vector<LuaReference> wormRender;
	std::vector<LuaReference> viewportRender;
};

extern LuaContext lua;

extern LuaCallbacks luaCallbacks;

#endif //GUSANOS_GLUA_H
