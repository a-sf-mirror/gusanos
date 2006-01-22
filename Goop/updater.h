#ifndef GUSANOS_UPDATE_H
#define GUSANOS_UPDATE_H

#include <zoidcom.h>

// Manages file transfers to clients
class Updater
{
public:
	static ZCom_ClassID classID;
	
	Updater();
	void assignNetworkRole( bool authority );
	void think();
	void removeNode();
	
private:
	ZCom_Node* m_node;
	bool m_isAuthority;
};

extern Updater updater;

#endif //GUSANOS_UPDATE_H
