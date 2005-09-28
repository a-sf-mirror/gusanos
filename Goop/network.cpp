#include "network.h"

#include "server.h"
#include "client.h"
#include "game.h"
#include "gconsole.h"
#include "text.h"
#include "net_worm.h"
#include "base_player.h"

#ifndef DISABLE_ZOIDCOM

#include <string>
#include <iostream>
#include <zoidcom.h>

using namespace std;

Network network;

Network::Network()
{
	m_zcom = NULL;
	m_control = NULL;
	connCount = 0;
	m_reconnect = false;
}

Network::~Network()
{
	m_host = false;
	m_client = false;
	m_serverID = ZCom_Invalid_ID;
}

void Network::log(char const* msg)
{
	cerr << "ZOIDCOM: " << msg << endl;
}

void Network::init()
{
	m_zcom = new ZoidCom();
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
	;
}

void Network::update()
{
	if ( m_control )
	{
		m_control->ZCom_processInput(eZCom_NoBlock);
		m_control->ZCom_processOutput();
	}
	if( m_reconnect )
	{
		disconnect();
		connect( m_lastServerAddr );
		m_reconnect = false;
	}
}

void Network::registerClasses() // Factorization of class registering in client and server
{
	NetWorm::classID = m_control->ZCom_registerClass("worm");
	BasePlayer::classID = m_control->ZCom_registerClass("player");
	Game::classID = m_control->ZCom_registerClass("game");
}

void Network::host()
{
	disconnect();
	m_control = new Server(m_serverPort);
	registerClasses();
	m_host = true;
	game.assignNetworkRole( true ); // Gives the game class node authority role
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

#endif






