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
	static LuaReference metaTable;
	
	LuaEventDef(std::string name_, LuaReference callb_)
	: name(name_), idx(0), callb(callb_)
	{
	}
	
	~LuaEventDef();
	
	void call(ZCom_BitStream*);
	
	void call(LuaReference, ZCom_BitStream*);
	
	void* operator new(size_t count);
	
	void operator delete(void* block)
	{
		// Lua frees the memory
	}
	
	void* operator new(size_t count, void* space)
	{
		return space;
	}
	
	std::string  name;
	size_t       idx;
	LuaReference callb;
	LuaReference luaReference;
};

class Network
{
public:

	static int const protocolVersion;
		
	enum NetEvents
	{
		PLAYER_REQUEST,
		RConMsg,
		ConsistencyInfo,
	};
	
	enum DConnEvents
	{
		Kick,
		ServerMapChange,
		Quit,
		Incompatible,
	};
	
	struct ClientEvents
	{
		enum type
		{
			LuaEvents,
			Max
		};
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
	static void disconnect( ZCom_ConnID id, DConnEvents event );
	static void reconnect();
	static void clear();
	
	static void kick( ZCom_ConnID connId );
	
	static void setServerID( ZCom_ConnID serverID );
	static ZCom_ConnID getServerID();
	
	static bool isHost();
	static bool isClient();
	
	static HTTP::Request* fetchServerList();
	static void addHttpRequest(HTTP::Request*, HttpRequestCallback);
	
	static LuaEventDef* addLuaEvent(LuaEventGroup::type, char const* name, LuaEventDef* event);
	static void indexLuaEvent(LuaEventGroup::type, char const* name);
	static LuaEventDef* indexToLuaEvent(LuaEventGroup::type type, int idx);
	static void encodeLuaEvents(ZCom_BitStream* data);
	
	static ZCom_Control* getZControl();
	static int getServerPing();

	static void incConnCount();
	static void decConnCount();
	
	int simLag;
	float simLoss;
	int upLimit;
	int downBPP;
	int downPPS;
	int checkCRC;
};

extern Network network;

#endif

#endif // _NETWORK_H_
