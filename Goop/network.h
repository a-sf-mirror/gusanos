#ifndef NETWORK_H
#define NETWORK_H


#ifndef DISABLE_ZOIDCOM

#include <zoidcom.h>
#include <string>

class Network
{
	public:
		
	Network();
	~Network();
	
	void init();
	void shutDown();
	void registerInConsole();
	void update();
	
	void host();
	void connect( const std::string &address);
	void disconnect();
	
	bool isHost();
	bool isClient();

	private:
		
	bool m_host;
	bool m_client;
	
	int m_serverPort;
	
	ZoidCom* m_zcom;
	ZCom_Control* m_control;
};

extern Network network;

#endif

#endif // _NETWORK_H_
