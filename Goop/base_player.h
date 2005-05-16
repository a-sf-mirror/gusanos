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
		NINJAROPE
	};
	
	enum NetEvents
	{
		ACTION_STOP,
		ACTION_START
	};

	static ZCom_ClassID  classID;
	
	BasePlayer();
	virtual ~BasePlayer();
	
	void think();
	virtual void subThink() = 0;
	virtual void render() {};

	void assignNetworkRole( bool authority );
	void setOwnerId( ZCom_ConnID id );

	void assignWorm(BaseWorm* worm);
	
	void baseActionStart( BaseActions action );
	void baseActionStop( BaseActions action );

	ZCom_NodeID getNodeID();
	
	PlayerOptions* getOptions();
	Vec getPos();
	
	protected:
	
	int deaths;
	int kills;

	BaseWorm* m_worm;
	PlayerOptions* m_options;

	bool m_isAuthority;
	ZCom_Node *m_node;
	BasePlayerInterceptor* m_interceptor;
	ZCom_NodeID m_wormID;
	ZCom_ConnID id; // NOTE: Why is this here??
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
