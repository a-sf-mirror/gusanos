#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "base_object.h"
#include "base_action.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <list>
#include <map>

class BasePlayer;
class PlayerOptions;
class WeaponType;
class PartType;
class Font;

static const int MAX_LOCAL_PLAYERS = 2;

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
	int worm_weaponHeight;
	int worm_height;
	int worm_maxClimb;
	int host;
};

class Game
{
	public:
		
	Game();
	~Game();
	
	void init(int argc, char** argv);
	void parseCommandLine(int argc, char** argv);
	
	void setMod(const std::string& mod);
	void loadWeapons();
	void unload();
	void loadMod();
	bool isLoaded();
	void changeLevel(const std::string& level);
	void addBot();
	
	Level level;
	std::vector<WeaponType*> weaponList;
	Options options;
	std::vector<PlayerOptions*> playerOptions;
	
	std::vector<Player*> localPlayers;
	std::vector<BasePlayer*> players;
	std::list<BaseObject*> objects;
	
	std::map< std::string, BaseAction*(*)( const std::vector< std::string > & ) > actionList;
	
	PartType* NRPartType;
	
	const std::string& getMod();
	
	template <typename T1>
	bool specialLoad(const std::string& name, T1 &resource)
	{
		if ( resource.load(level.getPath() + name) ) return true;
		if ( resource.load(m_modPath + name) ) return true;
		if ( resource.load(m_defaultPath + name) ) return true;
		
		return false;
	}
	
	Font *infoFont;
	
	private:
		
	std::string nextMod;
	std::string m_modPath;
	std::string m_modName;
	std::string m_defaultPath;
	bool loaded;

};

extern Game game;

#endif // _GAME_H_
