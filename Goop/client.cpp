#include "client.h"
#include "gconsole.h"

#ifndef DISABLE_ZOIDCOM

#include <zoidcom.h>

Client::Client( int _udpport )
{
	if ( !ZCom_initSockets( true,_udpport, 1, 0 ) )
	{
		console.addLogMsg("* ERROR: FAILED TO INITIALIZE SOCKETS");
	}
	ZCom_setControlID(0);
	ZCom_setDebugName("ZCOM_CLI");
}

Client::~Client()
{
}

void Client::ZCom_cbConnectResult( ZCom_ConnID _id, eZCom_ConnectResult _result, ZCom_BitStream &_reply )
{
	if ( _result != eZCom_ConnAccepted )
	{
		console.addLogMsg("* THE CONNECTION WAS REJECTED BY SERVER");
	}
	else
	{
		console.addLogMsg("* THE CONNECTION WAS ACCEPTED");
		game.setMod( _reply.getStringStatic() );
		game.changeLevel( _reply.getStringStatic() );
	}
} 

void Client::ZCom_cbConnectionClosed( ZCom_ConnID _id, ZCom_BitStream &_reason )
{
	console.addLogMsg("* CONNECTION CLOSED BY SERVER");
}

#endif