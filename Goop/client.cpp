#include "client.h"
#include "gconsole.h"
#include "net_worm.h"
#include "game.h"

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
		ZCom_requestZoidMode(_id, 1);
		game.setMod( _reply.getStringStatic() );
		game.changeLevel( _reply.getStringStatic() );
	}
} 

void Client::ZCom_cbConnectionClosed( ZCom_ConnID _id, ZCom_BitStream &_reason )
{
	console.addLogMsg("* CONNECTION CLOSED BY SERVER");
}

void Client::ZCom_cbZoidResult(ZCom_ConnID _id, eZCom_ZoidResult _result, zU8 _new_level, ZCom_BitStream &_reason)
{
	if (_result != eZCom_ZoidEnabled)
	{
		console.addLogMsg("* ERROR: COULDNT ENTER ZOIDMODE");
	}
	console.addLogMsg("* JOINED ZOIDMODE 1");
}

void Client::ZCom_cbNodeRequest_Dynamic(ZCom_ConnID _id, ZCom_ClassID _requested_class, eZCom_NodeRole _role, ZCom_NodeID _net_id)
{
	// check the requested class
	if ( _requested_class == NetWorm::classID )
	{
		console.addLogMsg("WORM NODE REQUESTED");
		if(true)
		{
			NetWorm* worm = new NetWorm(false);
			game.objects.push_back( worm );
			game.objects.push_back( (BaseObject*)worm->getNinjaRopeObj() );
		}
	}else
	{
		console.addLogMsg("INVALID DYNAMIC NODE REQUEST");
	}
	
}

#endif


