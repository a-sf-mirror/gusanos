#include "network.h"
#include "server.h"
#include "client.h"
#include "gconsole.h"
#include "text.h"
#include "net_worm.h"
#include "base_player.h"

#ifndef DISABLE_ZOIDCOM

#include <string>
#include <zoidcom.h>

using namespace std;

Network network;

Network::Network()
{
	m_zcom = NULL;
	m_control = NULL;
}

Network::~Network()
{
	m_host = false;
	m_client = false;
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
	if ( m_control ) delete m_control;
	if ( m_zcom ) delete m_zcom;
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
}

void Network::disconnect()
{
	if ( m_control ) delete m_control;
	m_client = false;
	m_host = false;
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

#endif






