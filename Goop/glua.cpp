#include "glua.h"
#include "player.h"
#include "luaapi/context.h"

//LuaContext lua;

LuaCallbacks luaCallbacks;

void LuaCallbacks::bind(std::string callback, LuaReference ref)
{
	#define CB(x_) else if(callback == #x_) x_.push_back(ref)
	if(callback == "afterRender")
		afterRender.push_back(ref);
	else if(callback == "afterUpdate")
		afterUpdate.push_back(ref);
	else if(callback == "wormRender")
		wormRender.push_back(ref);
	else if(callback == "viewportRender")
		viewportRender.push_back(ref);
	else if(callback == "localplayerLeft")
		localplayerEvent[Player::LEFT].push_back(ref);
	else if(callback == "localplayerRight")
		localplayerEvent[Player::RIGHT].push_back(ref);
	else if(callback == "localplayerUp")
		localplayerEvent[Player::UP].push_back(ref);
	else if(callback == "localplayerDown")
		localplayerEvent[Player::DOWN].push_back(ref);
	else if(callback == "localplayerJump")
		localplayerEvent[Player::JUMP].push_back(ref);
	else if(callback == "localplayerFire")
		localplayerEvent[Player::FIRE].push_back(ref);
	else if(callback == "localplayerChange")
		localplayerEvent[Player::CHANGE].push_back(ref);
	else if(callback == "localplayerInit")
		localplayerInit.push_back(ref);
	else if(callback == "wormDeath")
		wormDeath.push_back(ref);
	else if(callback == "playerUpdate")
		playerUpdate.push_back(ref);
	else if(callback == "playerInit")
		playerInit.push_back(ref);
	else if(callback == "localplayerEvent")
		localplayerEventAny.push_back(ref);
	CB(wormRemoved);
	CB(playerNetworkInit);
	CB(playerRemoved);
	CB(gameNetworkInit);
	CB(gameEnded);
}

void LuaObject::pushLuaReference()
{
	lua.push(getLuaReference());
}

void LuaObject::makeReference()
{
	lua_pushnil(lua);
}

LuaReference LuaObject::getLuaReference()
{
	if(luaReference)
		return luaReference;
	else
	{
		makeReference();
		luaReference = lua.createReference();
		return luaReference;
	}
}

void LuaObject::deleteThis()
{
	finalize();
	
	if(luaReference)
	{
		lua.destroyReference(luaReference);
		luaReference.reset();
	}
	else
		delete this;
}
