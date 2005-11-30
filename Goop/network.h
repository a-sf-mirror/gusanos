#ifndef NETWORK_H
#define NETWORK_H


#ifndef DISABLE_ZOIDCOM

#include <zoidcom.h>
#include <string>
#include <boost/function.hpp>
#include "luaapi/types.h"

const unsigned int INVALID_NODE_ID = 0;

namespace HTTP { struct Request; }

typedef boost::function<void (HTTP::Request*)> HttpRequestCallback;

struct LuaEventDef
{
	LuaEventDef(size_t idx_, LuaReference callb_)
	: idx(idx_), callb(callb_)
	{
	}
	
	int       idx;
	LuaReference callb;
};

class Network
{
public:
		
	enum NetEvents
	{
		PLAYER_REQUEST,
		RConMsg,
	};
	
	enum DConnEvents
	{
		Kick,
		ServerMapChange,
		Quit
	};
	
	struct LuaEventGroup
	{
		enum type
		{
			Game,
			Player,
			Worm,
			Max
		};
	};
	
	Network();
	~Network();
	
	static void log(char const* msg);
	
	static void init();
	static void shutDown();
	static void registerInConsole();
	static void update();
	
	static void host();
	static void connect( const std::string &address);
	static void disconnect( DConnEvents event = Quit );
	static void reconnect();
	
	static void kick( ZCom_ConnID connId );
	
	static void setServerID( ZCom_ConnID serverID );
	static ZCom_ConnID getServerID();
	
	static bool isHost();
	static bool isClient();
	
	static HTTP::Request* fetchServerList();
	static void addHttpRequest(HTTP::Request*, HttpRequestCallback);
	
	static LuaEventDef* addLuaEvent(LuaEventGroup::type, char const* name, LuaReference);
	static LuaEventDef* indexToLuaEvent(LuaEventGroup::type type, int idx);
	
	static ZCom_Control* getZControl();
	static int getServerPing();

	static void incConnCount();
	static void decConnCount();
};

extern Network network;

#endif

#endif // _NETWORK_H_
