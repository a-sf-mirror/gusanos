#include "base_player.h"
#include "worm.h"
#include "net_worm.h"
#include "player_options.h"
#include "objects_list.h"

#include <zoidcom.h>
#include "network.h"
#include <allegro.h>
#include <list>

using namespace std;

ZCom_ClassID BasePlayer::classID = ZCom_Invalid_ID;

BasePlayer::BasePlayer()
{
	deaths = 0;
	kills = 0;
	
	m_worm = NULL;
	m_wormID = INVALID_NODE_ID;
	m_node = NULL;
	m_interceptor = NULL;
	m_isAuthority = false;
}

BasePlayer::~BasePlayer()
{
	if (m_interceptor) delete m_interceptor;
}

void BasePlayer::think()
{
	subThink();
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
					case ACTION_START: // ACTION TART LOL TBH
					{
						BaseActions action = (BaseActions)data->getInt(8);
						baseActionStart(action);
					}
					break;
					case ACTION_STOP:
					{
						BaseActions action = (BaseActions)data->getInt(8);
						baseActionStop(action);
					}
					break;
				}
			}
		}
	}
}

void BasePlayer::assignWorm(BaseWorm* worm)
{
	m_worm = worm;
	worm->assignOwner( this );
	if ( NetWorm* netWorm = dynamic_cast<NetWorm*>(worm))
	{
		m_wormID = netWorm->getNodeID();
	}
}

PlayerOptions *BasePlayer::getOptions()
{
	return m_options;
}

void BasePlayer::assignNetworkRole( bool authority )
{
	m_node = new ZCom_Node();
	if (!m_node)
	{
		allegro_message("ERROR: Unable to create player node.");
	}

	m_node->beginReplicationSetup();
		m_node->addReplicationInt( (zS32*)&deaths, 32, false, ZCOM_REPFLAG_MOSTRECENT | ZCOM_REPFLAG_INTERCEPT, ZCOM_REPRULE_AUTH_2_ALL , 0);
		m_node->addReplicationInt( (zS32*)&m_wormID, 32, false, ZCOM_REPFLAG_MOSTRECENT | ZCOM_REPFLAG_INTERCEPT, ZCOM_REPRULE_AUTH_2_ALL , INVALID_NODE_ID);
	m_node->endReplicationSetup();

	m_interceptor = new BasePlayerInterceptor( this );
	m_node->setReplicationInterceptor(m_interceptor);

	m_isAuthority = authority;
	if( m_isAuthority)
	{
		if( !m_node->registerNodeDynamic(classID, network.getZControl() ) )
		allegro_message("ERROR: Unable to register player authority node.");
	}else
	{
		if( !m_node->registerRequestedNode( classID, network.getZControl() ) )
		allegro_message("ERROR: Unable to register player requested node.");
	}

	m_node->applyForZoidLevel(1);
}

void BasePlayer::setOwnerId( ZCom_ConnID id )
{
	m_node->setOwner( id, true );
}

ZCom_NodeID BasePlayer::getNodeID()
{
	if (m_node)
		return m_node->getNetworkID();
	else
		return INVALID_NODE_ID;
}

Vec BasePlayer::getPos()
{
	return m_worm->getPos(); // WARNING: The player could not own a worm if he is a spectator.
}

BasePlayerInterceptor::BasePlayerInterceptor( BasePlayer* parent )
{
	m_parent = parent;
}

bool BasePlayerInterceptor::inPreUpdateItem(ZCom_Node *_node, ZCom_ConnID _from, eZCom_NodeRole _remote_role, const RepInfo &info)
{
	int recievedID = *static_cast<zU32*>(info.data_ptr_new);
	ObjectsList::Iterator objIter;
	for ( objIter = game.objects.begin(); (bool)objIter; ++objIter)
	{
		if ( NetWorm* worm = dynamic_cast<NetWorm*>(*objIter) )
		{
			if ( worm->getNodeID() == recievedID )
			{
				m_parent->assignWorm(worm);
			}
		}
	}
	return true;
}

void BasePlayer::baseActionStart ( BaseActions action )
{
	switch (action)
	{
		case LEFT:
		{
			if ( m_worm )
			{
				m_worm -> actionStart(Worm::MOVELEFT);
			}
			if ( m_node )
			{
				ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
				data->addInt(static_cast<int>(ACTION_START),8 );
				data->addInt(static_cast<int>(LEFT),8 );
				m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, data);
			}
		}
		break;
		
		case RIGHT:
		{
			if ( m_worm )
			{
					m_worm -> actionStart(Worm::MOVERIGHT);
			}
			if ( m_node )
			{
				ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
				data->addInt(static_cast<int>(ACTION_START),8 );
				data->addInt(static_cast<int>(RIGHT),8 );
				m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, data);
			}
		}
		break;
		
		case FIRE:
		{
			if ( m_worm )
			{
				m_worm -> actionStart(Worm::FIRE);
			}
			if ( m_node )
			{
				ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
				data->addInt(static_cast<int>(ACTION_START),8 );
				data->addInt(static_cast<int>(FIRE),8 );
				m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, data);
			}
		}
		break;
		
		case JUMP:
		{
			if ( m_worm )
			{
				m_worm -> actionStart(Worm::JUMP);
			}
			if ( m_node )
			{
				ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
				data->addInt(static_cast<int>(ACTION_START),8 );
				data->addInt(static_cast<int>(JUMP),8 );
				m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, data);
			}
		}
		break;
		
		case NINJAROPE:
		{
			if ( m_worm )
			{
				m_worm->actionStart(Worm::NINJAROPE);
			}
			if ( m_node )
			{
				ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
				data->addInt(static_cast<int>(ACTION_START),8 );
				data->addInt(static_cast<int>(NINJAROPE),8 );
				m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, data);
			}
		}
		break;
		
		case RESPAWN:
		{
			if ( m_worm )
			{
				m_worm->respawn();
			}
			if ( m_node )
			{
				ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
				data->addInt(static_cast<int>(ACTION_START),8 );
				data->addInt(static_cast<int>(RESPAWN),8 );
				m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, data);
				// I am sending the event to both the auth and the proxies, but I 
				//cant really think of any use for the proxies knowing this, maybe 
				//I should change it to only send to auth? :o
			}
		}
		break;
	}
}

void BasePlayer::baseActionStop ( BaseActions action )
{
	switch (action)
	{
		case LEFT:
		{
			if ( m_worm )
			{
				m_worm -> actionStop(Worm::MOVELEFT);
			}
			if ( m_node )
			{
				ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
				data->addInt(static_cast<int>(ACTION_STOP),8 );
				data->addInt(static_cast<int>(LEFT),8 );
				m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, data);
			}
		}
		break;
		
		case RIGHT:
		{
			if ( m_worm )
			{
				m_worm -> actionStop(Worm::MOVERIGHT);
			}
			if ( m_node )
			{
				ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
				data->addInt(static_cast<int>(ACTION_STOP),8 );
				data->addInt(static_cast<int>(RIGHT),8 );
				m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, data);
			}
		}
		break;
		
		case FIRE:
		{
			if ( m_worm )
			{
				m_worm -> actionStop(Worm::FIRE);
			}
			if ( m_node )
			{
				ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
				data->addInt(static_cast<int>(ACTION_STOP),8 );
				data->addInt(static_cast<int>(FIRE),8 );
				m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, data);
			}
		}
		break;
		
		case JUMP:
		{
			if ( m_worm )
			{
				m_worm -> actionStop(Worm::JUMP);
			}
			if ( m_node )
			{
				ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
				data->addInt(static_cast<int>(ACTION_STOP),8 );
				data->addInt(static_cast<int>(JUMP),8 );
				m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, data);
			}
		}
		break;
		
		case NINJAROPE:
		{
			if ( m_worm )
			{
				m_worm->actionStop(Worm::NINJAROPE);
			}
			if ( m_node )
			{
				ZCom_BitStream *data = ZCom_Control::ZCom_createBitStream();
				data->addInt(static_cast<int>(ACTION_STOP),8 );
				data->addInt(static_cast<int>(NINJAROPE),8 );
				m_node->sendEvent(ZCom_Node::eEventMode_ReliableOrdered, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH, data);
			}
		}
	}
}


