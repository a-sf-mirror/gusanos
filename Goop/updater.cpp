#include "updater.h"
#ifdef MAP_DOWNLOADING
#include "network.h"
#include "level.h"
#include "util/log.h"
#include "message_queue.h"
#include "util/text.h"
#include <list>
#include <string>
#include <map>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
namespace fs = boost::filesystem;
#endif

Updater updater;
ZCom_ClassID Updater::classID;

#ifdef MAP_DOWNLOADING
namespace
{
	enum Message
	{
		MsgRequestLevel = 0,
		MsgHello
	};
	
	mq_define_message(RequestLevel, 0, (std::string name))
		: name(name)
		{
			
		}
		
		std::string name;
	mq_end_define_message()
	
	struct ConnData
	{
		std::list<std::string> fileQueue;
		
		void sendOne(ZCom_ConnID connID);
		void queuePath(fs::path const& p);
		void queueLevel(std::string const& level);
	};
	
	std::map<ZCom_ConnID, ConnData> connections;
	
	MessageQueue msg;
	
	ZCom_Node* node = 0;
	bool isAuthority = false;
	bool ready = false;
	
	void ConnData::sendOne(ZCom_ConnID connID)
	{
		if(!fileQueue.empty())
		{
			std::string const& file = fileQueue.front();
			fileQueue.pop_front();
			ZCom_FileTransID fid = node->sendFile(file.c_str(), 0, connID, 0, 1.0f);
			ILOG("Sending file with ID " << fid);
		}
	}
	
	void ConnData::queuePath(fs::path const& p)
	{
		if(fs::is_directory(p))
		{
			fs::directory_iterator i(p), e;
			
			for(; i != e; ++i)
			{
				queuePath(*i);
			}
		}
		else
		{
			DLOG("Queing " << p.string());
			fileQueue.push_back(p.native_file_string());
		}
	}
	
	void ConnData::queueLevel(std::string const& level) 
	{
		// TODO: WARNING: Prevent exception throwing if level doesn't exist
		fs::path const& p = levelLocator.getPathOf(level);
		
		queuePath(p);
	}
}
#endif

Updater::Updater()
{
}

void Updater::assignNetworkRole( bool authority )
{
#ifdef MAP_DOWNLOADING
	assert(!node);
	node = new ZCom_Node;

	isAuthority = authority;
	if( authority)
	{
		node->setEventNotification(true, false); // Enables the eEvent_Init.
		if( !node->registerNodeUnique(classID, eZCom_RoleAuthority, network.getZControl() ) )
			ELOG("Unable to register updater authority node.");
		
		node->removeFromZoidLevel(1);
		node->applyForZoidLevel(2); // Updater operates at zoidlevel 2
	}
	else
	{
		if( !node->registerNodeUnique( classID, eZCom_RoleProxy, network.getZControl() ) )
			ELOG("Unable to register updater requested node.");
	}
#endif
}

void Updater::think()
{
#ifdef MAP_DOWNLOADING
	if( node )
	{
		if(ready)
		{
			mq_process_messages(msg)
				mq_case(RequestLevel)
					DLOG("Requesting level " << data.name);
					ZCom_BitStream* str = new ZCom_BitStream;
					str->addInt(MsgRequestLevel, 8);
					str->addString( data.name.c_str() );
					node->sendEventDirect(eZCom_ReliableOrdered, str, network.getServerID() );
					DLOG("Sent to ID " << network.getServerID());
				mq_end_case()
			mq_end_process_messages();
		}
	
		while ( node->checkEventWaiting() )
		{
			eZCom_Event    type;
			eZCom_NodeRole remote_role;
			ZCom_ConnID    conn_id;
			
			ZCom_BitStream* data = node->getNextEvent(&type, &remote_role, &conn_id);
			switch(type)
			{
				case eZCom_EventFile_Incoming:
				{
					ZCom_FileTransID fid = static_cast<ZCom_FileTransID>(data->getInt(ZCOM_FTRANS_ID_BITS));
					
					ZCom_FileTransInfo const& info = node->getFileInfo(conn_id, fid);
					
					fs::path p(info.path);
					
					fs::create_directories(p.branch_path());
					
					ILOG("Accepting incoming file with ID " << fid);
					node->acceptFile(conn_id, fid, 0, true);
				}
				break;
				
				case eZCom_EventFile_Complete:
				{
					ZCom_FileTransID fid = static_cast<ZCom_FileTransID>(data->getInt(ZCOM_FTRANS_ID_BITS));
					ILOG("Transfer of file with ID " << fid << " complete");
					
					ConnData& c = connections[conn_id];
					c.sendOne(conn_id);
				}
				break;
				
				case eZCom_EventFile_Data:
				{
					ZCom_FileTransID fid = static_cast<ZCom_FileTransID>(data->getInt(ZCOM_FTRANS_ID_BITS));
					ZCom_FileTransInfo const& info = node->getFileInfo(conn_id, fid);
					
					DLOG("Transfer: " << double(info.bps) / 1000.0 << " kB/s, " << ((100 * info.transferred) / info.size) << "% done.");
					
					//ZCom_ConnStats const& state = network.getZControl()->ZCom_getConnectionStats(conn_id);
					
				}
				break;
				
				case eZCom_EventInit:
				{
					node->setConnectionSpecificRelevance(conn_id, 1.0f);
					ZCom_BitStream* str = new ZCom_BitStream;
					str->addInt(MsgHello, 8);
					node->sendEventDirect(eZCom_ReliableOrdered, str, conn_id );
				}
				break;
				
				case eZCom_EventUser:
				{
					int i = data->getInt(8);
					ILOG("Got user event: " << i);
					
					switch(i)
					{
						case MsgRequestLevel:
						{
							
							ConnData& c = connections[conn_id];
							c.queueLevel(data->getStringStatic() );
							c.sendOne(conn_id);
						}
						break;
						
						case MsgHello:
						{
							DLOG("Got hello from server, connection " << conn_id);
							ready = true;
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
#endif
}

void Updater::removeNode()
{
#ifdef MAP_DOWNLOADING
	delete node; node = 0;
	ready = false;
#endif
}

void Updater::requestLevel(std::string const& name)
{
#ifdef MAP_DOWNLOADING
	mq_queue(msg, RequestLevel, name);
#endif
}
