#ifndef SERVER_H
#define SERVER_H

#ifndef DISABLE_ZOIDCOM

#include <zoidcom.h>

class Server : public ZCom_Control
{
	public:
	
	Server( int _udpport );
	~Server();
	
	protected:
		
	// called on incoming connections
	bool ZCom_cbConnectionRequest( ZCom_ConnID _id, ZCom_BitStream &_request, ZCom_BitStream &_reply );
	// called when incoming connection has been established
	void ZCom_cbConnectionSpawned( ZCom_ConnID _id );
	// called when a connection closed
	void ZCom_cbConnectionClosed( ZCom_ConnID _id, ZCom_BitStream &_reason );
	
	bool ZCom_cbZoidRequest( ZCom_ConnID _id, zU8 _requested_level, ZCom_BitStream &_reason);
	
	void ZCom_cbZoidResult(ZCom_ConnID _id, eZCom_ZoidResult _result, zU8 _new_level, ZCom_BitStream &_reason);


	void ZCom_cbDataReceived( ZCom_ConnID _id, ZCom_BitStream &_data ) {}
	void ZCom_cbConnectResult( ZCom_ConnID _id, eZCom_ConnectResult _result, ZCom_BitStream &_reply ) {}
	void ZCom_cbNodeRequest_Dynamic( ZCom_ConnID _id, ZCom_ClassID _requested_class, eZCom_NodeRole _role, ZCom_NodeID _net_id ) {}
	void ZCom_cbNodeRequest_Tag( ZCom_ConnID _id, ZCom_ClassID _requested_class, eZCom_NodeRole _role, zU32 _tag ) {}
	void ZCom_cbDiscovered( const ZCom_Address & _addr, ZCom_BitStream &_reply )  {}
	bool ZCom_cbDiscoverRequest( const ZCom_Address &_addr, ZCom_BitStream &_request, ZCom_BitStream &_reply ) {return false;}
};

#endif

#endif // _SERVER_H_

