#ifndef NET_WORM_H
#define NET_WORM_H

#include "vec.h"
#include "base_object.h"
#include "base_worm.h"
//#include "sprite.h"

#include <zoidcom.h>

class NetWormInterceptor;

class NetWorm : public BaseWorm
{	
	public:
		
	enum NetEvents
	{
		PosCorrection = 0,
		Respawn,
		Dig,
		Die,
		ChangeWeapon,
		WeaponMessage,
		SYNC,
		EVENT_COUNT,
	};
		
	enum ReplicationItems
	{
		PlayerID = 0,
		Position,
		AIM
	};
		
	static ZCom_ClassID  classID;
	static const float MAX_ERROR_RADIUS = 10;
		
	NetWorm(bool isAuthority);
	~NetWorm();

	void think();
	void correctOwnerPosition();

	void assignOwner( BasePlayer* owner);
	void setOwnerId( ZCom_ConnID _id );
	void sendSyncMessage( ZCom_ConnID id );
	void sendWeaponMessage( int index, ZCom_BitStream* data );
	
	ZCom_NodeID getNodeID();
	
	void respawn();
	void dig();
	void die();
	void changeWeaponTo( unsigned int weapIndex );
	void damage( float amount, BasePlayer* damager );
	
	Vec lastPosUpdate;
	int timeSinceLastUpdate;
	
	private:
		
	bool m_isAuthority;
	ZCom_Node *m_node;
	NetWormInterceptor* m_interceptor;
	ZCom_NodeID m_playerID; // The id of the owner player node to replicate to all proxys
};

class NetWormInterceptor : public ZCom_NodeReplicationInterceptor
{
public:
	NetWormInterceptor( NetWorm* parent );

	bool inPreUpdateItem (ZCom_Node *_node, ZCom_ConnID _from, eZCom_NodeRole _remote_role, ZCom_Replicator *_replicator, zU32 _estimated_time_sent);

	// Not used virtual stuff
	void outPreReplicateNode(ZCom_Node *_node, ZCom_ConnID _to, eZCom_NodeRole _remote_role) {}
	void outPreDereplicateNode(ZCom_Node *_node, ZCom_ConnID _to, eZCom_NodeRole _remote_role) {}
	bool outPreUpdate(ZCom_Node *_node, ZCom_ConnID _to, eZCom_NodeRole _remote_role) { return true; }
	bool outPreUpdateItem (ZCom_Node *_node, ZCom_ConnID _to, eZCom_NodeRole _remote_role, ZCom_Replicator *_replicator) { return true; }
	void outPostUpdate(ZCom_Node *_node, ZCom_ConnID _to, eZCom_NodeRole _remote_role, zU32 _rep_bits, zU32 _event_bits, zU32 _meta_bits) {}
	bool inPreUpdate(ZCom_Node *_node, ZCom_ConnID _from, eZCom_NodeRole _remote_role) { return true; }
	void inPostUpdate(ZCom_Node *_node, ZCom_ConnID _from, eZCom_NodeRole _remote_role, zU32 _rep_bits, zU32 _event_bits, zU32 _meta_bits) {};


private:
	NetWorm* m_parent;
};

#endif  // _WORM_H_
