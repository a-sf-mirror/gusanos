#include "bindings-network.h"

#include "luaapi/types.h"

#include "../network.h"
#include "../game.h"
#include "../base_player.h"
#include "../base_worm.h"
#include "sockets.h"
#include "util/log.h"
#include "tcp.h"

#include <iostream>
#include <memory>
#include <utility>
#include <list>
#include <cstring>
using std::cerr;
using std::endl;
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
using boost::lexical_cast;

namespace LuaBindings
{
	
LuaReference socketMetaTable;
LuaReference bitStreamMetaTable;
LuaReference luaGameEventMetaTable, luaPlayerEventMetaTable, luaWormEventMetaTable;


class LuaSocket : public TCP::Socket
{
public:
	LuaSocket(int s_ /*LuaReference ref, int s_, LuaContext context_, LuaReference recvCallback_*/)
	: TCP::Socket(s_, 10*60) //, context(context_), recvCallback(recvCallback_)
	, dataSender(0) //, luaReference(ref)
	{
	}
	
	void send(char const* p, size_t len)
	{
		char* m = new char[len];
		memcpy(m, p, len);
		sendQueue.push_back(std::make_pair(m, m+len));
	}
	
	void think()
	{
		static bool d = true;
		bool r = d; d = false;
		
		if(r) DLOG("Start");
		
		if(error)
		{
			cerr << "Error!" << endl;
			error = ErrorNone;
		}
		
		if(r) DLOG("Before think()");
		TCP::Socket::think();
		if(r) DLOG("After think()");
		
		if(connected)
		{
			if(r) DLOG("Connected, sending data...");
			if(dataSender) // We're still sending data
			{
				if(dataSender->resume())
				{
					// Send done or errored
					delete dataSender; dataSender = 0;
					delete[] sendQueue.front().first;
					sendQueue.pop_front();
				}
			}
			else if(!sendQueue.empty())
			{
				std::pair<char*, char*>& t = sendQueue.front();
				
				if(!(dataSender = TCP::Socket::send(t.first, t.second)))
				{
					delete[] t.first;
					sendQueue.pop_front();
				}
			}
			
			if(r) DLOG("Reading chunk");
			if(readChunk())
				return;
		}
	}
	
	char const* begin()
	{
		return dataBegin;
	}
	
	char const* end()
	{
		return dataEnd;
	}
	
private:
	TCP::Socket::ResumeSend* dataSender;
	std::list<std::pair<char*, char*> > sendQueue;
	//LuaContext context;
	//LuaReference recvCallback;
	LuaReference luaReference;
};

/*! tcp_connect(addr, port)
	
	Returns a TCPSocket object connecting to //addr// on port //port//.
*/
int l_tcp_connect(lua_State* L)
{
	LuaContext context(L);
	
	char const* addr = lua_tostring(context, 1);
	if(!addr) return 0;
	int port = lua_tointeger(context, 2);
	
	//lua_pushvalue(context, 3);
	//LuaReference recvCallback = context.createReference();
	
	sockaddr_in server;
	
	std::auto_ptr<TCP::Hostent> hp(TCP::resolveHost( addr ));
	
	if(!hp.get())
		return 0;
	
    TCP::createAddr(server, hp.get(), port);
    
    int s;
    if((s = TCP::socketNonBlock()) < 0)
    	return 0;
    	
    if(!TCP::connect(s, server))
    	return 0;

	void* space = lua_newuserdata(context, sizeof(LuaSocket));
	//lua_pushvalue(context, -1);
	//LuaSocket* sock = new (space) LuaSocket(context.createReference(), s, context, recvCallback);
	new (space) LuaSocket(s);
	context.push(socketMetaTable);
	lua_setmetatable(context, -2);
	
	return 1;
}

/*! TCPSocket:send(data)
	
	Sends //data// on the socket.
*/
LMETHOD(LuaSocket, tcp_send,
	size_t len;
	char const* s = lua_tolstring(context, 2, &len);
	if(!s) return 0;
	p->send(s, len);
	return 0;
)

/*! TCPSocket:think()
	
	Processes data sending and recieving.
	If there's data recieved, it's returned as a string.
	If there's no data recieved, nil is returned. 
	Must be called regularly for data sends and connection to finish.
*/
LMETHOD(LuaSocket, tcp_think,
	p->think();
	if(p->begin() == p->end())
		return 0;
	DLOG((void *)p->begin() << ", " << (void *)p->end() << ", size: " << (p->end() - p->begin()));
	lua_pushlstring(context, p->begin(), p->end() - p->begin());
	return 1;
)

LMETHOD(LuaSocket, tcp_destroy,
	p->~LuaSocket();
	return 0;
)

METHOD(ZCom_BitStream, bitStream_addInt,
	int bits = 32;
	if(lua_gettop(context) >= 3)
		bits = lua_tointeger(context, 3);
	p->addInt(lua_tointeger(context, 2), bits);
	context.pushvalue(1);
	return 1;
)

METHOD(ZCom_BitStream, bitStream_getInt,
	int bits = 32;
	if(lua_gettop(context) >= 2)
		bits = lua_tointeger(context, 2);
	context.push(static_cast<int>(p->getInt(bits)));
	return 1;
)

METHOD(ZCom_BitStream, bitStream_addString,
	p->addString(lua_tostring(context, 2));
	context.pushvalue(1);
	return 1;
)

METHOD(ZCom_BitStream, bitStream_getString,
	context.push(p->getStringStatic());
	return 1;
)

METHOD(ZCom_BitStream, bitStream_destroy,
	delete p;
	return 0;
)

int l_new_bitstream(lua_State* L)
{
	LuaContext context(L);
	ZCom_BitStream* p = new ZCom_BitStream;
	context.pushFullReference(*p, bitStreamMetaTable);
	return 1;
}

#define LUA_EVENT_SEND_METHOD(type_, params_, decl_, cases_, body_) \
LMETHOD(LuaEventDef, luaEvent_##type_##_send, \
	if(p->idx >= 0)	{ \
		ZCom_BitStream** userdata = 0; \
		eZCom_SendMode mode = eZCom_ReliableOrdered; \
		ZCom_ConnID connID = 0; \
		zU8 rules = ZCOM_REPRULE_AUTH_2_ALL | ZCOM_REPRULE_OWNER_2_AUTH; \
		decl_ \
		switch(lua_gettop(context)) \
		{ \
			default: if(lua_gettop(context) < params_+3) break; \
			case params_+5: rules = lua_tointeger(context, params_+5); \
			case params_+4: mode = (eZCom_SendMode)lua_tointeger(context, params_+4); \
			case params_+3: connID = (ZCom_ConnID)lua_tointeger(context, params_+3); \
			case params_+2: userdata = static_cast<ZCom_BitStream**>(lua_touserdata(context, params_+2)); \
			cases_ \
		} \
		body_ \
	} \
	return 0; \
)

LUA_EVENT_SEND_METHOD(game, 0, /* no declarations */, /* no cases */,
	game.sendLuaEvent(p, mode, rules, userdata, connID);
)

LUA_EVENT_SEND_METHOD(player, 1,
	BasePlayer* player = 0;
,
	case 2: player = static_cast<BasePlayer*>(lua_touserdata(context, 2));
,
	if(player)
		player->sendLuaEvent(p, mode, rules, userdata, connID);
)

LUA_EVENT_SEND_METHOD(worm, 1,
	BaseWorm* worm = 0;
,
	case 2: worm = static_cast<BaseWorm*>(lua_touserdata(context, 2));
,
	if(worm)
		worm->sendLuaEvent(p, mode, rules, userdata, connID);
)

/*
LMETHOD(LuaEventDef, luaEvent_game_send,
	if(p->idx >= 0)
	{
		ZCom_BitStream** userdata = static_cast<ZCom_BitStream**>(lua_touserdata(context, 2));
		
		eZCom_SendMode mode = eZCom_ReliableOrdered;
		zU8 rules = ZCOM_REPRULE_AUTH_2_ALL | ZCOM_REPRULE_OWNER_2_AUTH;
		switch(lua_gettop(context))
		{
			case 4: rules = lua_tointeger(context, 4);	
			case 3: mode = (eZCom_SendMode)lua_tointeger(context, 3);
		}
		game.sendLuaEvent(p, mode, rules, userdata);
	}
	else
		DLOG("Message is not indexed for some reason");
	return 0;
)*/

int l_network_game_event(lua_State* L)
{
	LuaContext context(L);
	char const* name = lua_tostring(context, 1);
	if(!name) return 0;
	lua_pushvalue(context, 2);
	LuaReference ref = context.createReference();
	LuaEventDef* event = lua_new_m_keep(LuaEventDef, (name, ref), context, luaGameEventMetaTable);
	event = network.addLuaEvent(Network::LuaEventGroup::Game, name, event);

	DLOG("Registered game event: " << name);
	return 1;
}

int l_network_player_event(lua_State* L)
{
	LuaContext context(L);
	char const* name = lua_tostring(context, 1);
	if(!name) return 0;
	lua_pushvalue(context, 2);
	LuaReference ref = context.createReference();
	LuaEventDef* event = lua_new_m_keep(LuaEventDef, (name, ref), context, luaPlayerEventMetaTable);
	event = network.addLuaEvent(Network::LuaEventGroup::Player, name, event);

	DLOG("Registered player event: " << name);
	return 1;
}

int l_network_worm_event(lua_State* L)
{
	LuaContext context(L);
	char const* name = lua_tostring(context, 1);
	if(!name) return 0;
	lua_pushvalue(context, 2);
	LuaReference ref = context.createReference();
	LuaEventDef* event = lua_new_m_keep(LuaEventDef, (name, ref), context, luaWormEventMetaTable);
	event = network.addLuaEvent(Network::LuaEventGroup::Worm, name, event);

	DLOG("Registered game event: " << name);
	return 1;
}

void initNetwork(LuaContext& context)
{
	context.functions()
		("tcp_connect", l_tcp_connect)
		("network_game_event", l_network_game_event)
		("network_player_event", l_network_player_event)
		("network_worm_event", l_network_worm_event)
		("new_bitstream", l_new_bitstream)
	;
	
	CLASSM(socket,
		("__gc", l_tcp_destroy)
	,
		("send", l_tcp_send)
		("think", l_tcp_think)
	)
	
	CLASS(luaGameEvent,
		("send", l_luaEvent_game_send)
	)
	
	CLASS(luaPlayerEvent,
		("send", l_luaEvent_player_send)
	)
	
	CLASS(luaWormEvent,
		("send", l_luaEvent_worm_send)
	)
	
	CLASSM(bitStream,
		("__gc", l_bitStream_destroy)
	,
		("add_int", l_bitStream_addInt)
		("get_int", l_bitStream_getInt)
		("add_string", l_bitStream_addString)
		("get_string", l_bitStream_getString)
	)
	
	ENUM(SendMode,
		("ReliableUnordered", eZCom_ReliableUnordered)
		("ReliableOrdered", eZCom_ReliableOrdered)
		("Unreliable", eZCom_Unreliable)
	)
	
	ENUM(RepRule,
		("Auth2All", ZCOM_REPRULE_AUTH_2_ALL)
		("Auth2Owner", ZCOM_REPRULE_AUTH_2_OWNER)
		("Auth2Proxy", ZCOM_REPRULE_AUTH_2_PROXY)
		("None", ZCOM_REPRULE_NONE)
		("Owner2Auth", ZCOM_REPRULE_OWNER_2_AUTH)
	)
	
	lua_pushboolean(context, !network.isClient());
	lua_setfield(context, LUA_GLOBALSINDEX, "AUTH");
}

}
