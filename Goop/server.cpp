#include "server.h"
#include "gconsole.h"
#include "game.h"

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
	// request 20 packets/second and 200 bytes per packet from client (maximum values of course)
	console.addLogMsg("* CONNECTION SPAWNED");
	ZCom_requestDownstreamLimit(_id, 20, 200);
}

// called when a connection closed
void Server::ZCom_cbConnectionClosed( ZCom_ConnID _id, ZCom_BitStream &_reason )
{
	console.addLogMsg("* A CONNECTION WAS CLOSED");
}

