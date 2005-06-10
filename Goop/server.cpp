#include "server.h"
#include "gconsole.h"
#include "game.h"
#include "net_worm.h"
#include "base_worm.h"
#include "base_player.h"

#ifndef DISABLE_ZOIDCOM

#include <zoidcom.h>
#include <list.h>

Server::Server( int _udpport )
{
	if ( !ZCom_initSockets( true, _udpport, 1, 0 ) )
	{
		console.addLogMsg("* ERROR: FAILED TO INITIALIZE SOCKETS");
	}
	ZCom_setControlID(0);
	ZCom_setDebugName("ZCOM_CLI");
	console.addLogMsg("SERVER UP");
}

Server::~Server()
{	
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
	console.addLogMsg("* CREATING WORM AND PLAYER FOR THE NEW CONNECTION");
	BaseWorm* worm = game.addWorm(true);
	if ( NetWorm* netWorm = dynamic_cast<NetWorm*>(worm) )
	{
		netWorm->setOwnerId(_id);
	}
	BasePlayer* player = game.addPlayer ( Game::PROXY );
	player->assignNetworkRole(true);
	player->setOwnerId(_id);
	player->assignWorm(worm);
}

#endif

