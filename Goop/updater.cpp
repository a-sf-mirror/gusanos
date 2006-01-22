#include "updater.h"
#include "network.h"
#include "util/log.h"

Updater updater;
ZCom_ClassID Updater::classID;

Updater::Updater()
: m_node(0)
{
}

void Updater::assignNetworkRole( bool authority )
{
	assert(!m_node);
	m_node = new ZCom_Node;

	/*
	m_node->beginReplicationSetup(2);
	m_node->addReplicationInt( (zS32*)&options.worm_gravity, 32, false, ZCOM_REPFLAG_MOSTRECENT | ZCOM_REPFLAG_RARELYCHANGED, ZCOM_REPRULE_AUTH_2_ALL );
	m_node->addReplicationInt( (zS32*)&options.teamPlay, 1, false, ZCOM_REPFLAG_MOSTRECENT | ZCOM_REPFLAG_RARELYCHANGED, ZCOM_REPRULE_AUTH_2_ALL );
	
	m_node->endReplicationSetup();
	*/

	m_isAuthority = authority;
	if( authority)
	{
		m_node->setEventNotification(true, false); // Enables the eEvent_Init.
		if( !m_node->registerNodeUnique(classID, eZCom_RoleAuthority, network.getZControl() ) )
			ELOG("Unable to register updater authority node.");
		
		m_node->removeFromZoidLevel(1);
		m_node->applyForZoidLevel(2); // Updater operates at zoidlevel 2
		//m_node->applyForZoidLevel(1); // TEST
	}
	else
	{
		if( !m_node->registerNodeUnique( classID, eZCom_RoleProxy, network.getZControl() ) )
			ELOG("Unable to register updater requested node.");
	}
}

void Updater::think()
{
	while ( m_node && m_node->checkEventWaiting() )
	{
		eZCom_Event    type;
		eZCom_NodeRole remote_role;
		ZCom_ConnID    conn_id;
		
		ZCom_BitStream* data = m_node->getNextEvent(&type, &remote_role, &conn_id);
		switch(type)
		{
			case eZCom_EventFile_Incoming:
			{
				ZCom_FileTransID fid = static_cast<ZCom_FileTransID>(data->getInt(ZCOM_FTRANS_ID_BITS));
				ILOG("Accepting incoming file with ID " << fid);
				m_node->acceptFile(conn_id, fid, "test-recv.txt", true);
			}
			break;
			
			case eZCom_EventFile_Complete:
			{
				ZCom_FileTransID fid = static_cast<ZCom_FileTransID>(data->getInt(ZCOM_FTRANS_ID_BITS));
				ILOG("Transfer of file with ID " << fid << " complete");
			}
			break;
			
			case eZCom_EventInit:
			{
				ZCom_BitStream *data = new ZCom_BitStream;
				data->addInt(1, 32);
				m_node->sendEventDirect(eZCom_ReliableOrdered, data, conn_id );
			}
			break;
			
			case eZCom_EventUser:
			{
				int i = data->getInt(32);
				ILOG("Got user event: " << i);
				
				switch(i)
				{
					case 1:
					{
						ZCom_BitStream *data = new ZCom_BitStream;
						data->addInt(2, 32);
						m_node->sendEventDirect(eZCom_ReliableOrdered, data, conn_id );
					}
					break;
					
					case 2:
					{
						ZCom_FileTransID fid = m_node->sendFile("test.txt", 0, conn_id, 0, 0.5f);
						ILOG("Sending file with ID " << fid);
					}
					break;
				}
			}
			break;
			
			default:
				ILOG("Got some other event");
			break;
		}
	}
}

void Updater::removeNode()
{
	delete m_node; m_node = 0;
}
