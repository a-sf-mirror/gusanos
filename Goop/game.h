#ifndef GAME_H
#define GAME_H

#include "level.h"
//#include "base_object.h"
//#include "base_action.h"
#include "objects_list.h"
#include "hash_table.h"
#include "object_grid.h"

#ifndef DEDSERV
#include <allegro.h>
#endif
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
class Particle;
class PartType;
class Explosion;
#ifndef DEDSERV
class Font;
#endif


#define USE_GRID

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
	int worm_width;
	int worm_maxClimb;
	float worm_boxRadius;
	float worm_boxTop;
	float worm_boxBottom;
	int maxRespawnTime;
	int minRespawnTime;
	int host;
	int maxWeaponsVar;
	size_t maxWeapons;
	std::string rConPassword;
};

struct LevelEffectEvent
{
	LevelEffectEvent( int index_, int x_, int y_ ) : index(index_), x(x_), y(y_)
	{
	}
	int index;
	int x,y;
	
};

struct ScreenMessage
{
	enum Type
	{
		Death,
		Chat,
	};
	
	ScreenMessage(Type type_, std::string const& str_, int timeOut_ = 400)
	: type(type_), str(str_), timeOut(timeOut_)
	{
	}
	
	Type type;
	std::string str;
	int timeOut;
};

class Game
{
public:
#ifndef USE_GRID
	enum ColLayer
	{
		WORMS_COLLISION_LAYER = 0,
		NO_COLLISION_LAYER = 1,
		COLLISION_LAYER_COUNT = 10,
	};
	
	enum RenderLayer
	{
		WORMS_RENDER_LAYER = 4,
		RENDER_LAYER_COUNT = 10,
	};
	
	static const int CUSTOM_COL_LAYER_START = 2;
#endif

	static const int MAX_LOCAL_PLAYERS = 2;
	
	
		
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
	
	void think();
	
	bool setMod(const std::string& mod);
	void loadWeapons();
	void unload();
	void loadMod();
	bool isLoaded();
	void refreshResources();
	void refreshLevels();
	bool changeLevel(const std::string& level, bool refresh = true);
	bool changeLevelCmd(const std::string& level);
	void addBot();
	BasePlayer* findPlayerWithID( ZCom_NodeID ID );
	BasePlayer* addPlayer( PLAYER_TYPE player );
	BaseWorm* addWorm(bool isAuthority); // Creates a worm class depending on the network condition.
	
	void assignNetworkRole( bool authority );
	void removeNode();
	
	void applyLevelEffect( LevelEffect* effect, int x, int y );
	
	void sendRConMsg( std::string const & message );
	void displayChatMsg( std::string const& owner, std::string const& message);
	void displayKillMsg( BasePlayer* killed, BasePlayer* killer );
	void displayMessage( ScreenMessage const& msg );
	
	Level level;
	std::vector<WeaponType*> weaponList;
	Options options;
	std::vector<PlayerOptions*> playerOptions;
	
	std::vector<Player*> localPlayers;
	std::list<BasePlayer*> players;
#ifdef USE_GRID
	Grid objects;
#else
	ObjectsList objects;
#endif

	void insertExplosion( Explosion* explosion );
	
	std::map< std::string, BaseAction*(*)( const std::vector< std::string > & ) > actionList;
	//HashTable< std::string, BaseAction*(*)( const std::vector< std::string > & ) > actionList;
	
	PartType* NRPartType;
	PartType* deathObject;
	PartType* digObject;
	
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

#ifndef DEDSERV
	Font *infoFont;
#endif
	std::list<ScreenMessage> messages;
	

	HashTable<std::string, unsigned long> stringToIndexMap;
	std::vector<std::string> indexToStringMap;

	unsigned long stringToIndex(std::string const& str);
	
	std::string const& indexToString(unsigned long idx);
	
private:
	
	enum NetEvents
	{
		eHole = 0,
		// Add here
		NetEventsCount,
	};
	
	friend inline void addEvent(ZCom_BitStream* data, NetEvents event);
	
	std::list<LevelEffectEvent> appliedLevelEffects;

	std::string nextMod;
	std::string m_modPath;
	std::string m_modName;
	std::string m_defaultPath;
	bool loaded;
	ZCom_Node *m_node;
	bool m_isAuthority;
};

extern Game game;

#endif // _GAME_H_
