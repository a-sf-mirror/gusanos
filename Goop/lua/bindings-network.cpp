#include "bindings-network.h"

#include "luaapi/types.h"

#include "../network.h"
#include "sockets.h"
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
		if(error)
		{
			cerr << "Error!" << endl;
			error = ErrorNone;
		}
		
		TCP::Socket::think();
		
		if(connected)
		{
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
			
			if(readChunk())
				return;
			/*
			if(dataBegin != dataEnd)
			{
				(context.call(recvCallback), luaReference, std::make_pair(dataBegin, dataEnd))();
			}
			*/
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
	lua_pushlstring(context, p->begin(), p->end() - p->begin()); 
	return 1;
)

LMETHOD(LuaSocket, tcp_destroy,
	p->~LuaSocket();
	return 0;
)
	
void initNetwork(LuaContext& context)
{
	context.function("tcp_connect", l_tcp_connect);
	
	CLASSM(socket,
		("__gc", l_tcp_destroy)
	,
		("send", l_tcp_send)
		("think", l_tcp_think)
	)
}

}
