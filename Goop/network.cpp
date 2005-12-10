#include "network.h"

#include "server.h"
#include "client.h"
#include "game.h"
#include "glua.h"
#include "gconsole.h"
//#include "text.h"
#include "net_worm.h"
#include "base_player.h"
#include "particle.h"
#include "http.h"
#include "util/macros.h"
#include "lua/bindings-network.h"

#ifndef DISABLE_ZOIDCOM

#include <string>
#include <iostream>
#include <list>
#include <utility>
#include <zoidcom.h>
#include <boost/assign/list_inserter.hpp>
using namespace boost::assign;

using namespace std;

LuaReference LuaEventDef::metaTable;

namespace
{
	struct HttpRequest
	{
		HttpRequest(HTTP::Request* req_, HttpRequestCallback handler_)
		: req(req_), handler(handler_)
		{
		}
		
		HTTP::Request* req;
		HttpRequestCallback handler;
	};
	
	struct LuaEventList
	{
		typedef std::map<std::string, LuaEventDef*> MapT;
		MapT stringToEvent;
		std::vector<LuaEventDef*> events;
		
		LuaEventDef* add(std::string const& name, LuaReference ref)
		{
			int idx = events.size() + 1;
			LuaEventDef* event = lua_new(LuaEventDef, (idx, ref), lua);
			std::pair<MapT::iterator, bool> r = stringToEvent.insert(std::make_pair(name, event));
			if(!r.second)
			{
				r.first->second->idx = idx;
				delete event;
				event = r.first->second;
			}
			events.push_back(event);
			return event;
		}
		
		LuaEventDef* assign(std::string const& name, LuaReference ref)
		{
			LuaEventDef* event = lua_new(LuaEventDef, (-1, ref), lua);
			std::pair<MapT::iterator, bool> r = stringToEvent.insert(std::make_pair(name, event));
			if(!r.second)
			{
				r.first->second->callb = ref;
				delete event;
				event = r.first->second; 
			}
			return event;
		}
		
		LuaEventDef* fromIndex(int idx)
		{
			if(idx >= 0 && idx < events.size())
				return events[idx];
			return 0;
		}
	};
	
	std::list<HttpRequest> requests;
	std::string serverName;
	std::string serverDesc;
	HTTP::Host masterServer("comser.liero.org.pl");
	int updateTimer = 0;
	bool serverAdded = false;
	bool m_host = false;
	bool m_client = false; //? This wasn't initialized before
	
	int m_serverPort; // Neither was this
	
	std::string m_lastServerAddr;
	bool m_reconnect = false;
	int connCount = 0;
	
	ZoidCom* m_zcom = 0;
	ZCom_Control* m_control = 0;
	ZCom_ConnID m_serverID = ZCom_Invalid_ID;
	LuaEventList luaEvents[Network::LuaEventGroup::Max];
	
	void processHttpRequests()
	{
		foreach_delete(i, requests)
		{
			if(i->req->think())
			{
				if(i->handler)
					i->handler(i->req);
				else
					delete i->req;
				requests.erase(i);
			}
		}
	}
	
	void onServerRemoved(HTTP::Request* req)
	{
		if(req->success)
		{
			cout << "Unregistered from master server" << endl;
		}
		else
		{
			if(req->getError() == TCP::Socket::ErrorTimeout)
				cerr << "Unregistration from master server timed out" << endl;
			else
				cerr << "Failed to unregister from master server" << endl;
		}
		serverAdded = false;
		delete req;
	}
	
	void onServerAdded(HTTP::Request* req)
	{
		if(req->success)
		{
			serverAdded = true;
			cout << "Registered to master server" << endl;
		}
		else
		{
			serverAdded = false;
			if(req->getError() == TCP::Socket::ErrorTimeout)
				cerr << "Registration to master server timed out" << endl;
			else
				cerr << "Failed to register to master server" << endl;
		}
		delete req;
	}
	
	void onServerUpdate(HTTP::Request* req)
	{
		if(req->success)
		{
			cout << "Sent update to master server" << endl;
		}
		else
		{
			if(req->getError() == TCP::Socket::ErrorTimeout)
				cerr << "Master server update timed out" << endl;
			else
				cerr << "Failed to send update to master server" << endl;
			serverAdded = false; // Maybe the server was droped, try to reregister it
		}
		delete req;
	}
	
	void registerToMasterServer()
	{
		std::list<std::pair<std::string, std::string> > data;

		push_back(data)
			("action", "add")
			("title", serverName)
			("desc", serverDesc)
			("mod", game.getModName())
			("map", game.level.getName())
		;
		network.addHttpRequest(masterServer.post("gusserv.php", data), onServerAdded);
	}
	
	void registerClasses() // Factorization of class registering in client and server
	{
		NetWorm::classID = m_control->ZCom_registerClass("worm",0);
		BasePlayer::classID = m_control->ZCom_registerClass("player",0);
		Game::classID = m_control->ZCom_registerClass("game",0);
		Particle::classID = m_control->ZCom_registerClass("particle",ZCOM_CLASSFLAG_ANNOUNCEDATA);
	}
	
	std::string setProxy(std::list<std::string> const& args) 
	{
		if(args.size() >= 2)
		{
			let_(i, args.begin());
			std::string const& addr = *i++;
			int port = cast<int>(*i++);
			
			masterServer.options.setProxy(addr, port);
			
			return "";
		}
		return "NET_SET_PROXY"; //TODO: help
	}
}

Network network;

void LuaEventDef::call(ZCom_BitStream* s)
{
	(lua.call(callb), luaReference, lua.fullReference(*s, LuaBindings::bitStreamMetaTable))();
}

LuaEventDef::~LuaEventDef()
{
	lua.destroyReference(luaReference);
	luaReference.reset();
	lua.destroyReference(callb);
	callb.reset();
}

Network::Network()
{
/*
	m_host = false;
	m_zcom = NULL;
	m_control = NULL;
	connCount = 0;
	m_reconnect = false;*/
}

Network::~Network()
{
/* What's with this?
	m_host = false;
	m_client = false;
	m_serverID = ZCom_Invalid_ID;*/
}

void Network::log(char const* msg)
{
	cerr << "ZOIDCOM: " << msg << endl;
}

void Network::init()
{
	m_zcom = new ZoidCom(log);
	m_zcom->setLogLevel(2);
	if ( !m_zcom->Init() )
	{
		console.addLogMsg("* ERROR: UNABLE TO INITIALIZE ZOIDCOM NETWORK LIB");
	}else
	{
		console.addLogMsg("* ZOIDCOM NETWORK LIB INITIALIZED");
		console.addLogMsg("* FOR MORE INFO VISIT WWW.ZOIDCOM.COM");
	}
}

void Network::shutDown()
{
	delete m_control; m_control = 0;
	delete m_zcom; m_zcom = 0;
}

void Network::registerInConsole()
{
	console.registerVariables()
		("NET_SERVER_PORT", &m_serverPort, 9898)
		("NET_SERVER_NAME", &serverName, "Unnamed server")
		("NET_SERVER_DESC", &serverDesc, "")
	;
	
	console.registerCommands()
		("NET_SET_PROXY", setProxy) 
	;
}

ZCom_ConnID Network::getServerID()
{
	return m_serverID;
}

void Network::update()
{
	if ( m_control )
	{
		m_control->ZCom_processOutput();
		m_control->ZCom_processInput(eZCom_NoBlock);
	}
	if( m_reconnect )
	{
		disconnect();
		connect( m_lastServerAddr );
		m_reconnect = false;
	}
	if(m_host)
	{
		if(++updateTimer > 6000*3)
		{
			updateTimer = 0;
			if(!serverAdded)
			{
				registerToMasterServer();
			}
			else
			{
				std::list<std::pair<std::string, std::string> > data;
				push_back(data)
					("action", "update")
				;
				network.addHttpRequest(masterServer.post("gusserv.php", data), onServerUpdate);
			}
		}
	}
	
	processHttpRequests();
}



HTTP::Request* Network::fetchServerList()
{
	std::list<std::pair<std::string, std::string> > data;
	push_back(data)
		("action", "list")
	;
	return masterServer.post("gusserv.php", data);
}



void Network::host()
{
	disconnect();
	m_control = new Server(m_serverPort);
	registerClasses();
	m_host = true;
	game.assignNetworkRole( true ); // Gives the game class node authority role
	
	registerToMasterServer();
}

void Network::connect( const std::string &_address )
{
	disconnect();
	m_control = new Client( 0 );
	registerClasses();
	ZCom_Address address;
	address.setAddress( eZCom_AddressUDP, 0, ( _address + ":" + cast<string>(m_serverPort) ).c_str() );
	m_control->ZCom_Connect( address, NULL );
	m_client = true;
	m_lastServerAddr = _address;
	game.assignNetworkRole( false ); // Gives the game class node proxy role
}

void Network::disconnect( DConnEvents event )
{
	if(serverAdded)
	{
		std::list<std::pair<std::string, std::string> > data;
		data.push_back(std::make_pair("action", "remove"));
		network.addHttpRequest(masterServer.post("gusserv.php", data), onServerRemoved);
	}
	
	if(requests.size())
	{
		cout << "Waiting for HTTP requests to finish... " << std::flush;
		do
		{
			processHttpRequests();
			rest(2);
		}
		while(requests.size() > 0);
		cout << "done." << endl;
	}
	
	if ( m_control )
	{
		ZCom_BitStream *eventData = new ZCom_BitStream;
		eventData->addInt( static_cast<int>( event ), 8 );
		
		m_control->ZCom_disconnectAll(eventData);
		
		int count = 0;
		while ( count < 10 )
		{
			rest(50);
			m_control->ZCom_processOutput();
			++count;
		}
		m_control->Shutdown();
	}
	
	delete m_control;
	m_control = NULL;
	connCount = 0;
	m_client = false;
	m_host = false;
	m_serverID = ZCom_Invalid_ID;
	
	game.removeNode();
}

void Network::reconnect()
{
	m_reconnect = true;
}

void Network::kick( ZCom_ConnID connId )
{
	if( m_control )
	{
		ZCom_BitStream *eventData = new ZCom_BitStream;
		eventData->addInt( static_cast<int>( Kick ), 8 );
		m_control->ZCom_Disconnect( connId, eventData );
	}
}

void Network::setServerID(ZCom_ConnID serverID)
{
	m_serverID = serverID;
}

bool Network::isHost()
{
	return m_host;
}

bool Network::isClient()
{
	return m_client;
}

ZCom_Control* Network::getZControl()
{
	return m_control;
}

int Network::getServerPing()
{
	if( m_client )
	{
		if ( m_serverID ) 
			return m_control->ZCom_getConnectionStats(m_serverID).avg_ping;
	}
	return -1;
}

void Network::addHttpRequest(HTTP::Request* req, HttpRequestCallback handler)
{
	requests.push_back(HttpRequest(req, handler));
}

LuaEventDef* Network::addLuaEvent(LuaEventGroup::type type, char const* name, LuaReference ref)
{
	if(m_host)
		return luaEvents[type].add(name, ref);
	else if(m_client)
		return luaEvents[type].assign(name, ref);
	
	return 0;
}

LuaEventDef* Network::indexToLuaEvent(LuaEventGroup::type type, int idx)
{
	return luaEvents[type].fromIndex(idx);
}

void Network::incConnCount()
{
	++connCount;
}

void Network::decConnCount()
{
	--connCount;
}

#endif






