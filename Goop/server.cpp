#include "server.h"
#include "gconsole.h"
#include "game.h"

#ifndef DISABLE_ZOIDCOM

#include <zoidcom.h>

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

#endif

