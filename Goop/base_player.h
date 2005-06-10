#ifndef BASE_PLAYER_H
#define BASE_PLAYER_H

#include <string>
#include "vec.h"

#include <zoidcom.h>

struct PlayerOptions;
class BaseWorm;
class BasePlayerInterceptor;

class BasePlayer
{
	public:
	
	enum BaseActions
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		FIRE,
		JUMP,
		CHANGE,
		NINJAROPE,
		RESPAWN
	};
	
	enum NetEvents
	{
		SYNC,
		ACTION_STOP,
		ACTION_START
	};
	
	enum ReplicationItems
	{
		WormID,
		Other
	};

	static ZCom_ClassID  classID;
	
	BasePlayer();
	virtual ~BasePlayer();
	
	void think();
	virtual void subThink() = 0;
	virtual void render() {};

	void assignNetworkRole( bool authority );
	void setOwnerId( ZCom_ConnID id );
	void sendSyncMessage( ZCom_ConnID id );

	void assignWorm(BaseWorm* worm);
	void removeWorm();
	
	void baseActionStart( BaseActions action );
	void baseActionStop( BaseActions action );
	
	void addKill();
	void addDeath();

	ZCom_NodeID getNodeID();
	ZCom_ConnID getConnectionID();
	
	PlayerOptions* getOptions();
	Vec getPos();
	
	int deaths;
	int kills;
	
	bool deleteMe;
	
	protected:

	BaseWorm* m_worm;
	PlayerOptions* m_options;

	bool m_isAuthority;
	ZCom_Node *m_node;
	BasePlayerInterceptor* m_interceptor;
	ZCom_NodeID m_wormID;
	ZCom_ConnID m_id;
};

class BasePlayerInterceptor : public ZCom_NodeReplicationInterceptor
{
public:
	BasePlayerInterceptor( BasePlayer* parent );

        bool inPreUpdateItem(ZCom_Node *_node, ZCom_ConnID _from, eZCom_NodeRole _remote_role, const RepInfo &_info);

	// Not used virtual stuff
	void outPreReplicateNode(ZCom_Node *_node, ZCom_ConnID _to, eZCom_NodeRole _remote_role) {}
        void outPreDereplicateNode(ZCom_Node *_node, ZCom_ConnID _to, eZCom_NodeRole _remote_role) {}
        bool outPreUpdate(ZCom_Node *_node, ZCom_ConnID _to, eZCom_NodeRole _remote_role) { return true; }
        bool outPreUpdateItem(ZCom_Node *_node, ZCom_ConnID _to, eZCom_NodeRole _remote_role, const RepInfo &_info) { return true; }
        void outPostUpdate(ZCom_Node *_node, ZCom_ConnID _to, eZCom_NodeRole _remote_role, zU32 _rep_bits, zU32 _event_bits, zU32 _meta_bits) {}
        bool inPreUpdate(ZCom_Node *_node, ZCom_ConnID _from, eZCom_NodeRole _remote_role) { return true; }
        void inPostUpdate(ZCom_Node *_node, ZCom_ConnID _from, eZCom_NodeRole _remote_role, zU32 _rep_bits, zU32 _event_bits, zU32 _meta_bits) {};


private:
	BasePlayer* m_parent;
};

#endif  // _BASE_PLAYER_H_
