#include "net_worm.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#include "base_player.h"
#include "player_options.h"
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
		m_node->addInterpolationFloat((zFloat*)&renderPos.x,32,ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_ALL,99,0,(zFloat*)&pos.x,-1,-1,0);
		m_node->addInterpolationFloat((zFloat*)&renderPos.y,32,ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_ALL,99,0,(zFloat*)&pos.y,-1,-1,0);
		m_node->addReplicationFloat ((zFloat*)&aimAngle, 32, ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_ALL | ZCOM_REPRULE_OWNER_2_AUTH, 90, -1, 1000);
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
	if ( !m_isAuthority ) renderPos += (pos - renderPos)*0.1;
	else renderPos = pos;
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
	int recievedID = *static_cast<zU32*>(info.data_ptr_new);
	vector<BasePlayer*>::iterator playerIter;
	for ( playerIter = game.players.begin(); playerIter != game.players.end(); playerIter++)
	{
		if ( (*playerIter)->getNodeID() == recievedID )
		{
			m_parent->assignOwner(*playerIter);
		}
	}
	return true;
}
