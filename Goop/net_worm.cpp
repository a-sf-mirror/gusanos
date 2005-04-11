#include "net_worm.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#include "base_player.h"
#include "player_options.h"
#include "network.h"

#include <math.h>
#include <zoidcom.h>

ZCom_ClassID NetWorm::classID = ZCom_Invalid_ID;

NetWorm::NetWorm(bool isAuthority) : BaseWorm()
{
	m_node = new ZCom_Node();
	if (!m_node)
	{
		allegro_message("ERROR: Unable to create worm node.");
	}
	
	m_node->beginReplicationSetup();
		m_node->addInterpolationFloat((zFloat*)&renderPos.x,32,ZCOM_REPFLAG_MOSTRECENT,ZCOM_REPRULE_AUTH_2_ALL,99,20000,(zFloat*)&pos.x,-1,-1,0);
		m_node->addInterpolationFloat((zFloat*)&renderPos.y,32,ZCOM_REPFLAG_MOSTRECENT,ZCOM_REPRULE_AUTH_2_ALL,99,20000,(zFloat*)&pos.y,-1,-1,0);
	m_node->endReplicationSetup();

	m_isAuthority = isAuthority;
	if( isAuthority)
	{
		if( !m_node->registerNodeDynamic(classID, network.getZControl() ) )
		allegro_message("ERROR: Unable to register authority node.");
	}else
	{
		if( !m_node->registerRequestedNode( classID, network.getZControl() ) )
		allegro_message("ERROR: Unable to register requested node.");
	};
	
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

