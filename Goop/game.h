#ifndef GAME_H
#define GAME_H

#include "level.h"
//#include "base_object.h"
//#include "base_action.h"
#include "objects_list.h"
#include "lua/context.h"
#include "hash_table.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <zoidcom.h>

class BasePlayer;
class BaseWorm;
class BaseAction;
class PlayerOptions;
class WeaponType;
class PartType;
class Font;

static const int MAX_LOCAL_PLAYERS = 2;
static const int WORMS_COLLISION_LAYER = 0;
static const int WORMS_RENDER_LAYER = 4;

#define EACH_CALLBACK(i_, type_) for(std::vector<int>::iterator i_ = game.luaCallbacks.type_.begin(); \
			i_ != game.luaCallbacks.type_.end(); ++i_)
			
class Player;

struct Options
{
	void registerInConsole();
	
	float ninja_rope_shootSpeed;
	float ninja_rope_pullForce;
	float ninja_rope_startDistance;
	float worm_maxSpeed;
	float worm_acceleration;
	float worm_airAccelerationFactor;
	float worm_friction;
	float worm_airFriction;
	float worm_gravity;
	float worm_bounceQuotient;
	float worm_bounceLimit;
	float worm_jumpForce;
	int worm_disableWallHugging;
	int worm_weaponHeight;
	int worm_height;
	int worm_maxClimb;
	float worm_boxRadius;
	float worm_boxTop;
	float worm_boxBottom;
	int maxRespawnTime;
	int minRespawnTime;
	int host;
}; 

class Game
{
public:
		
	static ZCom_ClassID  classID;

	enum PLAYER_TYPE
	{
		OWNER = 0,
		PROXY,
		AI,
	};
		
	Game();
	~Game();
	
	void init(int argc, char** argv);
	void parseCommandLine(int argc, char** argv);
	
	void setMod(const std::string& mod);
	void loadWeapons();
	void unload();
	void loadMod();
	bool isLoaded();
	void refreshResources();
	void changeLevel(const std::string& level);
	void addBot();
	BasePlayer* findPlayerWithID( ZCom_NodeID ID );
	BasePlayer* addPlayer( PLAYER_TYPE player );
	BaseWorm* addWorm(bool isAuthority); // Creates a worm class depending on the network condition.
	
	Level level;
	std::vector<WeaponType*> weaponList;
	Options options;
	std::vector<PlayerOptions*> playerOptions;
	
	std::vector<Player*> localPlayers;
	std::list<BasePlayer*> players;
	
	ObjectsList objects;
	
	std::map< std::string, BaseAction*(*)( const std::vector< std::string > & ) > actionList;
	
	PartType* NRPartType;
	PartType* deathObject;
	
	const std::string& getMod();
	const std::string& getModPath();
	const std::string& getDefaultPath();
	
/*	//Not used anymore
	template <typename T1>
	bool specialLoad(const std::string& name, T1 &resource)
	{
		if ( resource.load(level.getPath() + name) ) return true;
		if ( resource.load(m_modPath + name) ) return true;
		if ( resource.load(m_defaultPath + name) ) return true;
		
		return false;
	}*/
	
	Font *infoFont;
	LuaContext lua;
	
	struct LuaCallbacks
	{
		void bind(std::string callback, std::string file, std::string function);
		void bind(std::string callback, int ref);
		std::vector<int> atGameStart;
		std::vector<int> afterRender;
		std::vector<int> afterUpdate;
		std::vector<int> wormRender;
		std::vector<int> viewportRender;
	} luaCallbacks;
	
	HashTable<std::string, unsigned long> stringToIndexMap;
	std::vector<std::string> indexToStringMap;

	unsigned long stringToIndex(std::string const& str);
	
	std::string const& indexToString(unsigned long idx);
	
private:

	std::string nextMod;
	std::string m_modPath;
	std::string m_modName;
	std::string m_defaultPath;
	bool loaded;
	ZCom_Node *m_node;
};

extern Game game;

#endif // _GAME_H_
