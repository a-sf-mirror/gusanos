#ifndef NET_WORM_H
#define NET_WORM_H

#include "vec.h"
#include "base_object.h"
#include "base_worm.h"
#include "sprite.h"

#include <zoidcom.h>

class NetWorm : public BaseWorm
{	
	public:
		
	static ZCom_ClassID  classID;
		
	NetWorm(bool isAuthority);
	~NetWorm();

	void think();
	
	private:

	bool m_isAuthority;
	ZCom_Node *m_node;
	ZCom_ConnID id;

};

#endif  // _WORM_H_
