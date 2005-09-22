#include "server.h"
#include "gconsole.h"
#include "game.h"
#include "net_worm.h"
#include "base_worm.h"
#include "base_player.h"
#include "network.h"

#ifndef DISABLE_ZOIDCOM

#include <zoidcom.h>
#include <allegro.h>
#include <list>

Server::Server( int _udpport )
{
	int tries = 10;
	bool result = false;
	while ( !result && tries-- > 0 )
	{
		rest(100);
		result = ZCom_initSockets( true, _udpport, 1, 0 );
	}
	if(!result)
		console.addLogMsg("* ERROR: FAILED TO INITIALIZE SOCKETS");
	ZCom_setControlID(0);
	ZCom_setDebugName("ZCOM_CLI");
	console.addLogMsg("SERVER UP");
}

Server::~Server()
{	
}

void Server::ZCom_cbDataReceived( ZCom_ConnID  _id, ZCom_BitStream &_data) 
{
	Network::NetEvents event = (Network::NetEvents) _data.getInt(8);
	switch( event )
	{
		case Network::PLAYER_REQUEST:
		{
			std::string name = _data.getStringStatic();
			
			console.addLogMsg( "* " + name + " HAS JOINED THE GAME");
			BaseWorm* worm = game.addWorm(true);
			if ( NetWorm* netWorm = dynamic_cast<NetWorm*>(worm) )
			{
				netWorm->setOwnerId(_id);
			}
			BasePlayer* player = game.addPlayer ( Game::PROXY );
			player->changeName( name );
			player->assignNetworkRole(true);
			player->setOwnerId(_id);
			player->assignWorm(worm);
		}
		break;
		case Network::RConMsg:
		{
			//console.addLogMsg( "RCON MESSAGE RECIEVED");
			if ( !game.options.rConPassword.empty() && game.options.rConPassword == _data.getStringStatic() )
			{
				console.parseLine(_data.getStringStatic());
			}
		}
		break;
	}
}

bool Server::ZCom_cbConnectionRequest( ZCom_ConnID _id, ZCom_BitStream &_request, ZCom_BitStream &_reply )
{
	console.addLogMsg("* CONNECTION REQUESTED");
	_reply.addString( game.getMod().c_str() );
	_reply.addString( game.level.getName().c_str() );
	return true;
}

void Server::ZCom_cbConnectionSpawned( ZCom_ConnID _id )
{
	console.addLogMsg("* CONNECTION SPAWNED");
	ZCom_requestDownstreamLimit(_id, 20, 200);
}

void Server::ZCom_cbConnectionClosed( ZCom_ConnID _id, ZCom_BitStream &_reason )
{
	console.addLogMsg("* A CONNECTION WAS CLOSED");
	for ( std::list<BasePlayer*>::iterator iter = game.players.begin(); iter != game.players.end(); iter++)
	{
		if ( (*iter)->getConnectionID() == _id )
		{
			(*iter)->deleteMe = true;
		}
	}
}

bool Server::ZCom_cbZoidRequest( ZCom_ConnID _id, zU8 _requested_level, ZCom_BitStream &_reason)
{
	if (_requested_level == 1)
	{
		console.addLogMsg("* ZOIDMODE REQUEST ACCEPTED");
		return true;
	}
	else
		return false;
}

void Server::ZCom_cbZoidResult(ZCom_ConnID _id, eZCom_ZoidResult _result, zU8 _new_level, ZCom_BitStream &_reason)
{
	console.addLogMsg("* NEW CONNECTION JOINED ZOIDMODE");
}

#endif

