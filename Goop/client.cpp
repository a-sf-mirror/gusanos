#include "client.h"
#include "gconsole.h"
#include "net_worm.h"
#include "base_player.h"
#include "game.h"
#include "network.h"
#include "player_options.h"

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

void Client::requestPlayer(PlayerOptions const& playerOptions)
{
	ZCom_BitStream *req = new ZCom_BitStream;
	req->addInt(Network::PLAYER_REQUEST,8);
	req->addString( playerOptions.name.c_str() );
	req->addInt(playerOptions.colour, 24);
	req->addInt(playerOptions.uniqueID, 32);
	std::cerr << "Sent ID: " << playerOptions.uniqueID << std::endl;
	ZCom_sendData( network.getServerID(), req, eZCom_ReliableOrdered );
}

void Client::requestPlayers()
{
	requestPlayer(*game.playerOptions[0]);
	if ( game.options.splitScreen )
		requestPlayer(*game.playerOptions[1]);
}

void Client::ZCom_cbConnectResult( ZCom_ConnID _id, eZCom_ConnectResult _result, ZCom_BitStream &_reply )
{
	if ( _result != eZCom_ConnAccepted )
	{
		console.addLogMsg("* COULDNT ESTABLISH CONNECTION");
	}
	else
	{
		console.addLogMsg("* CONNECTION ACCEPTED");
		network.setServerID(_id);
		ZCom_requestZoidMode(_id, 1);
		game.setMod( _reply.getStringStatic() );
		game.changeLevel( _reply.getStringStatic() );
		++network.connCount;
	}
} 

void Client::ZCom_cbConnectionClosed( ZCom_ConnID _id, ZCom_BitStream &_reason )
{
	--network.connCount;
	Network::DConnEvents dcEvent = static_cast<Network::DConnEvents>( _reason.getInt(8) );
	switch( dcEvent )
	{
		case Network::ServerMapChange:
		{
			console.addLogMsg("* SERVER CHANGED MAP");
			network.reconnect();
		}
		break;
		case Network::Quit:
		{
			console.addLogMsg("* CONNECTION CLOSED BY SERVER");
		}
		break;
		case Network::Kick:
		{
			console.addLogMsg("* YOU WERE KICKED");
		}
		break;
		default:
		{
			console.addLogMsg("* CONNECTION CLOSED BY DUNNO WHAT :O");
		}
		break;
	}
}

void Client::ZCom_cbZoidResult(ZCom_ConnID _id, eZCom_ZoidResult _result, zU8 _new_level, ZCom_BitStream &_reason)
{
	if (_result != eZCom_ZoidEnabled)
	{
		console.addLogMsg("* ERROR: COULDNT ENTER ZOIDMODE");
	}else
	{
		console.addLogMsg("* JOINED ZOIDMODE");
		requestPlayers();
	}
}

void Client::ZCom_cbNodeRequest_Dynamic(ZCom_ConnID _id, ZCom_ClassID _requested_class, eZCom_NodeRole _role, ZCom_NodeID _net_id)
{
	// check the requested class
	if ( _requested_class == NetWorm::classID )
	{
		if(true)
		{
			game.addWorm(false);
		}
	}else if ( _requested_class == BasePlayer::classID )
	{
		// Creates a player class depending on the role
		if( _role == eZCom_RoleOwner )
		{
			BasePlayer* player = game.addPlayer ( Game::OWNER );
			player->assignNetworkRole(false);
		}else
		{
			BasePlayer* player = game.addPlayer ( Game::PROXY );
			player->assignNetworkRole(false);
		}
	}else
	{
		console.addLogMsg("* ERROR: INVALID DYNAMIC NODE REQUEST");
	}
	
}

#endif



