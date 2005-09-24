#include "net_worm.h"

#include "vec.h"
#include "game.h"
#include "weapon.h"
#include "base_worm.h"
#include "base_object.h"
#include "base_player.h"
#include "player_options.h"
#include "ninjarope.h"
#include "network.h"
#include "vector_replicator.h"
#include "posspd_replicator.h"
#include "encoding.h"
#include "gconsole.h"

#include <math.h>
#include <vector>
#include <zoidcom.h>

using namespace std;

ZCom_ClassID NetWorm::classID = ZCom_Invalid_ID;

NetWorm::NetWorm(bool isAuthority) : BaseWorm()
{
	timeSinceLastUpdate = 1;
	
	m_playerID = INVALID_NODE_ID;
	m_node = new ZCom_Node();
	if (!m_node)
	{
		allegro_message("ERROR: Unable to create worm node.");
	}
	
	m_node->beginReplicationSetup(0);
	
		static ZCom_ReplicatorSetup posSetup( ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH , (int)Position, -1, 1000);
		
		m_node->addReplicator(new PosSpdReplicator( &posSetup, &pos, &spd, game.level.vectorEncoding ), true);
		
		static ZCom_ReplicatorSetup nrSetup( ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH );
		
		m_node->addReplicator(new VectorReplicator( &nrSetup, &m_ninjaRope->getPosReference(), game.level.vectorEncoding ), true);
		
		static ZCom_ReplicatorSetup angleSetup( ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_PROXY | ZCOM_REPRULE_OWNER_2_AUTH );
				
		m_node->addReplicationFloat ((zFloat*)&health, 16, ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_ALL);
		
		m_node->addReplicator(new AngleReplicator( &angleSetup, &aimAngle), true );
		
		// Intercepted stuff
		m_node->setInterceptID( static_cast<ZCom_InterceptID>(PlayerID) );
		
		m_node->addReplicationInt( (zS32*)&m_playerID, 32, false, ZCOM_REPFLAG_MOSTRECENT | ZCOM_REPFLAG_INTERCEPT, ZCOM_REPRULE_AUTH_2_ALL , INVALID_NODE_ID);
		
	m_node->endReplicationSetup();

	m_interceptor = new NetWormInterceptor( this );
	m_node->setReplicationInterceptor(m_interceptor);

	m_isAuthority = isAuthority;
	if( isAuthority)
	{
		m_node->setEventNotification(true, false); // Enables the eEvent_Init.
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
	delete m_node;
	delete m_interceptor;
}

inline void addEvent(ZCom_BitStream* data, int event)
{
#ifdef COMPACT_EVENTS
	//data->addInt(event, Encoding::bitsOf(NetWorm::EVENT_COUNT - 1));
	Encoding::encode(*data, event, NetWorm::EVENT_COUNT);
#else
	data->addInt(static_cast<int>(event),8 );
#endif
}

void NetWorm::think()
{
	BaseWorm::think();
#ifndef DEDSERV
	renderPos += (pos - renderPos)*0.2;
#endif

	++timeSinceLastUpdate;
	
	if ( !m_node )
		return;
	
	while ( m_node->checkEventWaiting() )
	{
		ZCom_Node::eEvent type;
		eZCom_NodeRole    remote_role;
		ZCom_ConnID       conn_id;
		
		ZCom_BitStream *data = m_node->getNextEvent(&type, &remote_role, &conn_id);
		switch(type)
		{
		case ZCom_Node::eEvent_User:
			if ( data )
			{
#ifdef COMPACT_EVENTS
				//NetEvents event = (NetEvents)data->getInt(Encoding::bitsOf(EVENT_COUNT - 1));
				NetEvents event = (NetEvents)Encoding::decode(*data, EVENT_COUNT);
#else
				NetEvents event = (NetEvents)data->getInt(8);
#endif
				switch ( event )
				{
					case PosCorrection:
					{
						/*
						pos.x = data->getFloat(32);
						pos.y = data->getFloat(32);
						spd.x = data->getFloat(32);
						spd.y = data->getFloat(32);*/
						pos = game.level.vectorEncoding.decode<Vec>(*data);
						spd = game.level.vectorEncoding.decode<Vec>(*data);
					}
					break;
					case Respawn:
					{
						Vec newpos = game.level.vectorEncoding.decode<Vec>(*data);
						//newpos.x = data->getFloat(32);
						//newpos.y = data->getFloat(32);
						BaseWorm::respawn( newpos );
					}
					break;
					case Dig:
					{
						Vec digPos = game.level.vectorEncoding.decode<Vec>(*data);
						Angle digAngle = Angle((int)data->getInt(Angle::prec));
						BaseWorm::dig(digPos, digAngle);
					}
					break;
					case Die:
					{
						m_lastHurt = game.findPlayerWithID( data->getInt(32) );
						BaseWorm::die();
					}
					break;
					case ChangeWeapon:
					{
						//size_t weapIndex = data->getInt(Encoding::bitsOf(game.weaponList.size() - 1));
						size_t weapIndex = Encoding::decode(*data, game.weaponList.size());
						changeWeaponTo( weapIndex );
					}
					break;
					case WeaponMessage:
					{
						//size_t weapIndex = data->getInt(Encoding::bitsOf(game.weaponList.size() - 1));
						size_t weapIndex = Encoding::decode(*data, game.weaponList.size());
						if ( weapIndex < m_weapons.size() )
							m_weapons[weapIndex]->recieveMessage( data );
					}
					break;
					case SYNC:
					{
						m_isActive = data->getBool();
						m_ninjaRope->active = data->getBool();
						//currentWeapon = data->getInt(Encoding::bitsOf(game.weaponList.size() - 1));
						currentWeapon = Encoding::decode(*data, game.weaponList.size());
					}
					break;
				}
			}
			break;
			
			case ZCom_Node::eEvent_Init:
			{
				sendSyncMessage( conn_id );
			}
			break;
			
			default: break; // Annoying warnings >:O
		}
	}
}

void NetWorm::correctOwnerPosition()
{
	ZCom_BitStream *data = new ZCom_BitStream;
	addEvent(data, PosCorrection);
	/*
	data->addFloat(pos.x,32); // Maybe this packet is too heavy...
	data->addFloat(pos.y,32);
	data->addFloat(spd.x,32);
	data->addFloat(spd.y,32);*/
	game.level.vectorEncoding.encode<Vec>(*data, pos); // ...nah ;o
	game.level.vectorEncoding.encode<Vec>(*data, spd);
	m_node->sendEvent(eZCom_ReliableOrdered, ZCOM_REPRULE_AUTH_2_OWNER, data);
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


void NetWorm::sendSyncMessage( ZCom_ConnID id )
{
	ZCom_BitStream *data = new ZCom_BitStream;
	addEvent(data, SYNC);
	data->addBool(m_isActive);
	data->addBool(m_ninjaRope->active);
	//data->addInt(currentWeapon, Encoding::bitsOf(game.weaponList.size() - 1));
	Encoding::encode(*data, currentWeapon, game.weaponList.size());
	m_node->sendEventDirect(eZCom_ReliableOrdered, data, id);
}

void NetWorm::sendWeaponMessage( int index, ZCom_BitStream* weaponData )
{
	ZCom_BitStream *data = new ZCom_BitStream;
	addEvent(data, WeaponMessage);
	//data->addInt(index, Encoding::bitsOf(game.weaponList.size() - 1));
	Encoding::encode(*data, index, game.weaponList.size());
	data->addBitStream( weaponData );
	m_node->sendEvent(eZCom_ReliableOrdered, ZCOM_REPRULE_AUTH_2_ALL, data);
}

ZCom_NodeID NetWorm::getNodeID()
{
	if ( m_node )
		return m_node->getNetworkID();
	else
		return INVALID_NODE_ID;
}

void NetWorm::respawn()
{
	if ( m_isAuthority && m_node )
	{
		BaseWorm::respawn();
		if ( m_isActive )
		{
			ZCom_BitStream *data = new ZCom_BitStream;
			addEvent(data, Respawn);
			/*
			data->addFloat(pos.x,32);
			data->addFloat(pos.y,32);*/
			game.level.vectorEncoding.encode<Vec>(*data, pos);
			m_node->sendEvent(eZCom_ReliableOrdered, ZCOM_REPRULE_AUTH_2_ALL, data);
		}
	}
}

void NetWorm::dig()
{
	if ( m_isAuthority && m_node )
	{
		BaseWorm::dig();
		if ( m_isActive )
		{
			ZCom_BitStream *data = new ZCom_BitStream;
			addEvent(data, Dig);
			game.level.vectorEncoding.encode<Vec>(*data, pos);
			data->addInt(int(getAngle()), Angle::prec);
			m_node->sendEvent(eZCom_ReliableOrdered, ZCOM_REPRULE_AUTH_2_ALL, data);
		}
	}
}

void NetWorm::die()
{
	if ( m_isAuthority && m_node )
	{
		ZCom_BitStream *data = new ZCom_BitStream;
		addEvent(data, Die);
		if ( m_lastHurt )
		{
			data->addInt( static_cast<int>( m_lastHurt->getNodeID() ), 32 );
		}
		else
		{
			data->addInt( INVALID_NODE_ID, 32 );
		}
		m_node->sendEvent(eZCom_ReliableOrdered, ZCOM_REPRULE_AUTH_2_ALL, data);
		BaseWorm::die();
	}
}

void NetWorm::changeWeaponTo( unsigned int weapIndex )
{
	if ( m_node )
	{
		ZCom_BitStream *data = new ZCom_BitStream;
		addEvent(data, ChangeWeapon);
		data->addInt(weapIndex, Encoding::bitsOf(game.weaponList.size() - 1));
		m_node->sendEvent(eZCom_ReliableOrdered, ZCOM_REPRULE_OWNER_2_AUTH | ZCOM_REPRULE_AUTH_2_PROXY, data);
		BaseWorm::changeWeaponTo( weapIndex );
	}
}

void NetWorm::damage( float amount, BasePlayer* damager )
{
	if ( m_isAuthority )
	{
		BaseWorm::damage( amount, damager );
	}
}

NetWormInterceptor::NetWormInterceptor( NetWorm* parent )
{
	m_parent = parent;
}

bool NetWormInterceptor::inPreUpdateItem (ZCom_Node *_node, ZCom_ConnID _from, eZCom_NodeRole _remote_role, ZCom_Replicator *_replicator, zU32 _estimated_time_sent)
{
	bool returnValue = false;
	switch ( (NetWorm::ReplicationItems) _replicator->getSetup()->getInterceptID() )
	{
		case NetWorm::PlayerID:
		{
			ZCom_NodeID recievedID = *static_cast<zU32*>(_replicator->peekData());
			list<BasePlayer*>::iterator playerIter;
			for ( playerIter = game.players.begin(); playerIter != game.players.end(); playerIter++)
			{
				if ( (*playerIter)->getNodeID() == recievedID )
				{
					(*playerIter)->assignWorm(m_parent);
				}
			}
			returnValue = true;
		} break;
		/*case NetWorm::Position:
		{
			Vec recievedPos = *static_cast<Vec*>(_replicator->peekData());
			Vec speedPrediction = (recievedPos - m_parent->lastPosUpdate) / m_parent->timeSinceLastUpdate;
			m_parent->lastPosUpdate = recievedPos;
			m_parent->timeSinceLastUpdate = 0;
			m_parent->spd = m_parent->spd*0.2 + speedPrediction*0.8;
			returnValue = true;
		} break;*/
	}
	return returnValue;
}

