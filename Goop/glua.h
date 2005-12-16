#ifndef GUSANOS_GLUA_H
#define GUSANOS_GLUA_H

#include <string>
#include <vector>
#include "luaapi/context.h"
#include "luaapi/types.h"

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
	std::vector<LuaReference> wormDeath;
	std::vector<LuaReference> wormRemoved;
	std::vector<LuaReference> playerUpdate;
	std::vector<LuaReference> playerInit;
	std::vector<LuaReference> playerNetworkInit;
	
	std::vector<LuaReference> localplayerEvent[7];
	std::vector<LuaReference> localplayerInit;
};

extern LuaContext lua;

extern LuaCallbacks luaCallbacks;

// This is GCC specific, because I can't find a way to do it in standard C++ :/
#define LUA_NEW(t_, param_) \
({ \
	void* space = lua.pushObject(t_::metaTable(), sizeof(t_)); \
	t_* p = new (space) t_ param_; \
	p->luaReference = lua.createReference(); \
	p; \
})

/*
#define LUA_DELETE(t_, p_) { \
	t_* p = (p_); \
	p->~t_(); \
	lua.destroyReference(p->luaReference); \
}*/

#endif //GUSANOS_GLUA_H
