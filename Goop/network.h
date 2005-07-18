#ifndef NETWORK_H
#define NETWORK_H


#ifndef DISABLE_ZOIDCOM

#include <zoidcom.h>
#include <string>

static unsigned int INVALID_NODE_ID = 0;

class Network
{
	public:
		
	enum NetEvents
	{
		PLAYER_REQUEST
	};
		
	Network();
	~Network();
	
	void init();
	void shutDown();
	void registerInConsole();
	void update();
	
	void host();
	void connect( const std::string &address);
	void disconnect();
	
	void setServerID( ZCom_ConnID serverID );
	ZCom_ConnID getServerID()
	{
		return m_serverID;
	}
	
	bool isHost();
	bool isClient();
	
	ZCom_Control* getZControl();
	int getServerPing();

	private:

	void registerClasses();
		
	bool m_host;
	bool m_client;
	
	int m_serverPort;
	
	ZoidCom* m_zcom;
	ZCom_Control* m_control;
	ZCom_ConnID m_serverID;
};

extern Network network;

#endif

#endif // _NETWORK_H_
