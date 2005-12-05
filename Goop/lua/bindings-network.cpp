#include "bindings-network.h"

#include "luaapi/types.h"

#include "../network.h"
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

class LuaSocket : public TCP::Socket
{
public:
	LuaSocket(int s_ /*LuaReference ref, int s_, LuaContext context_, LuaReference recvCallback_*/)
	: TCP::Socket(s_) //, context(context_), recvCallback(recvCallback_)
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

LMETHOD(LuaSocket, tcp_send,
	size_t len;
	char const* s = lua_tolstring(context, 2, &len);
	if(!s) return 0;
	p->send(s, len);
	return 0;
)

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

LMETHOD(LuaEventDef, luaEvent_send,
	
	return 0;
)

int l_network_game_event(lua_State* L)
{
	LuaContext context(L);
	char const* name = lua_tostring(context, 1);
	if(!name) return 0;
	lua_pushvalue(context, 2);
	LuaEventDef* event = network.addLuaEvent(Network::LuaEventGroup::Game, name, context.createReference());
	context.push(event->luaReference);
	return 1;
}

void initNetwork(LuaContext& context)
{
	context.functions()
		("tcp_connect", l_tcp_connect)
		("network_game_event", l_network_game_event)
	;
	
	CLASSM(socket,
		("__gc", l_tcp_destroy)
	,
		("send", l_tcp_send)
		("think", l_tcp_think)
	)
	
	CLASS_(LuaEventDef,
		//("send", )
	)
	
	CLASS(bitStream,
		("add_int", l_bitStream_addInt)
		("get_int", l_bitStream_getInt)
	)
}

}
