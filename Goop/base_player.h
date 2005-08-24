#ifndef BASE_PLAYER_H
#define BASE_PLAYER_H

#include <string>
#include "vec.h"
#include "lua/context.h"

#include <zoidcom.h>

struct PlayerOptions;
class BaseWorm;
class BasePlayerInterceptor;

// Note: None of the BaseActions should assume a combination of keys.
// For example: Activating JUMP and CHANGE does nothing here ( instead 
// of shooting the Ninja Rope ) So key combinations should be created
// on the Player class instead. Because of that, all actions in the 
// BasePlayer class are direct ( they do nothing more and nothing less
// than what the name tells )

// Note2: All access to the worm class from a derivation of BasePlayer
// should pass by the BasePlayer class ( This is because the BasePlayer
// class will be responsible of the network part )

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
		CHANGE, // Probably useless Action
		NINJAROPE,
		RESPAWN
	};
	
	enum NetEvents
	{
		SYNC,
		ACTION_STOP,
		ACTION_START,
		NAME_CHANGE,
		NAME_PETITION
	};
	
	enum ReplicationItems
	{
		WormID,
		Other
	};

	// ClassID is Used by zoidcom to identify the class over the network,
	// do not confuse with the node ID which identifies instances of the class.
	static ZCom_ClassID  classID;
	
	BasePlayer();
	virtual ~BasePlayer();
	
	void think();
	// subThink() gets called inside think() and its used to give the derivations
	// the ability to think without replacing the main BasePlayer::think().
	virtual void subThink() = 0;
	virtual void render() {};

	void assignNetworkRole( bool authority );
	void setOwnerId( ZCom_ConnID id );

	void assignWorm(BaseWorm* worm);
	void removeWorm();
	
	void sendSyncMessage( ZCom_ConnID id ); // Its the initializing message that is sent to new clients that recieve the node.
	
	void nameChangePetition(); // Asks the server to change the name to the one in the player options.
	void changeName( const std::string& name ); // Changes the name and if its server it will tell all clients about it.
	
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
	
	std::string m_name;
	
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
	BasePlayer* m_parent;
};

#endif  // _BASE_PLAYER_H_
