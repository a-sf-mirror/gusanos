#include "net_worm.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#include "base_player.h"
#include "player_options.h"
#include "ninjarope.h"
#include "network.h"

#include <math.h>
#include <vector>
#include <zoidcom.h>

using namespace std;

ZCom_ClassID NetWorm::classID = ZCom_Invalid_ID;

NetWorm::NetWorm(bool isAuthority) : BaseWorm()
{
	m_playerID = INVALID_NODE_ID;
	m_node = new ZCom_Node();
	if (!m_node)
	{
		allegro_message("ERROR: Unable to create worm node.");
	}
	
	m_node->beginReplicationSetup();
// 		m_node->addInterpolationFloat((zFloat*)&renderPos.x,32,ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH,99,0,(zFloat*)&pos.x,-1,-1,0);
// 		m_node->addInterpolationFloat((zFloat*)&renderPos.y,32,ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH,99,0,(zFloat*)&pos.y,-1,-1,0);
		
		m_node->addInterpolationFloat((zFloat*)&pos.x,32,ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH,99,200,NULL,-1,-1,1);
		m_node->addInterpolationFloat((zFloat*)&pos.y,32,ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH,99,200,NULL,-1,-1,1);
		
		m_node->addReplicationFloat ((zFloat*)&aimAngle, 32, ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, 90, -1, 1000);
		
		m_node->addReplicationFloat ((zFloat*)&m_ninjaRope->getPosRefference().x, 32, ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, 90, -1, 1000);
		
		m_node->addReplicationFloat ((zFloat*)&m_ninjaRope->getPosRefference().y, 32, ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, 90, -1, 1000);
		
		// Intercepted stuff
		m_node->setInterceptID( static_cast<ZCom_InterceptID>(PlayerID) );
		
		m_node->addReplicationInt( (zS32*)&m_playerID, 32, false, ZCOM_REPFLAG_MOSTRECENT | ZCOM_REPFLAG_INTERCEPT, ZCOM_REPRULE_AUTH_2_ALL , INVALID_NODE_ID);
		
	m_node->endReplicationSetup();

	m_interceptor = new NetWormInterceptor( this );
	m_node->setReplicationInterceptor(m_interceptor);

	m_isAuthority = isAuthority;
	if( isAuthority)
	{
		if( !m_node->registerNodeDynamic(classID, network.getZControl() ) )
		allegro_message("ERRORBasePlayer: Unable to register worm authority node.");
	}else
	{
		if( !m_node->registerRequestedNode( classID, network.getZControl() ) )
		allegro_message("ERROR: Unable to register worm requested node.");
	}
	
	m_node->applyForZoidLevel(1);
}

NetWorm::~NetWorm()
{
	if ( m_node ) delete m_node;
}

void NetWorm::think()
{
	BaseWorm::think();
	/*if ( !m_isAuthority ) */renderPos += (pos - renderPos)*0.2;
// 	else renderPos = pos;
	
	if ( m_node )
	{
		while ( m_node->checkEventWaiting() )
		{
			ZCom_Node::eEvent type;
			eZCom_NodeRole    remote_role;
			ZCom_ConnID       conn_id;
			
			ZCom_BitStream *data = m_node->getNextEvent(&type, &remote_role, &conn_id);
			if (type == ZCom_Node::eEvent_User && data)
			{
				NetEvents event = (NetEvents)data->getInt(8);
				switch ( event )
				{
					case PosCorrection:
					{
						pos.x = data->getFloat(32);
						pos.y = data->getFloat(32);
						spd.x = data->getFloat(32);
						spd.y = data->getFloat(32);
					}
					break;
				}
			}
		}
	}
}

void NetWorm::correctOwnerPosition()
{
	ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
	data->addInt(static_cast<int>(PosCorrection),8 );
	data->addFloat(pos.x,32); // Maybe this packet is too heavy...
	data->addFloat(pos.y,32);
	data->addFloat(spd.x,32);
	data->addFloat(spd.y,32);
	m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_OWNER, data);
}

void NetWorm::assignOwner( BasePlayer* owner)
{
	BaseWorm::assignOwner(owner);
	m_playerID = m_owner->getNodeID();
}

void NetWorm::setOwnerId( ZCom_ConnID _id )
{
	m_node->setOwner(_id,true);
}

ZCom_NodeID NetWorm::getNodeID()
{
	if ( m_node )
		return m_node->getNetworkID();
	else
		return INVALID_NODE_ID;
}

NetWormInterceptor::NetWormInterceptor( NetWorm* parent )
{
	m_parent = parent;
}

bool NetWormInterceptor::inPreUpdateItem(ZCom_Node *_node, ZCom_ConnID _from, eZCom_NodeRole _remote_role, const RepInfo &info)
{
	bool returnValue;
	switch ( (NetWorm::ReplicationItems) info.id )
	{
		case NetWorm::PlayerID:
		{
			int recievedID = *static_cast<zU32*>(info.data_ptr_new);
			vector<BasePlayer*>::iterator playerIter;
			for ( playerIter = game.players.begin(); playerIter != game.players.end(); playerIter++)
			{
				if ( (*playerIter)->getNodeID() == recievedID )
				{
					m_parent->assignOwner(*playerIter);
				}
			}
			returnValue = true;
		} break;
	}
	return returnValue;
}
