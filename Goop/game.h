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

static const int MAX_LOCAL_PLAYERS = 2;

class Player;

struct Options
{
	void registerInConsole();
	
	float worm_maxSpeed;
	float worm_acceleration;
	float worm_friction;
	float worm_airFriction;
	float worm_gravity;
	float worm_bounceQuotient;
	float worm_bounceLimit;
	float worm_jumpForce;
	int worm_weaponHeight;
	int worm_height;
	int worm_maxClimb;
};

class Game
{
	public:
		
	Game();
	~Game();
	
	void init();
	
	const std::vector<std::string>& getPaths();
	void addPath( const std::string& path);
	void removePath();
	
	Level level;
	
	std::vector<Player*> localPlayers;
	std::vector<PlayerOptions*> playerOptions;
	std::vector<BasePlayer*> players;
	std::list<BaseObject*> objects;
	Options options;
	
	
	std::map< std::string, BaseAction*(*)( const std::vector< std::string > & ) > actionList;
	
	private:
	std::vector<std::string> m_paths;
};

extern Game game;

template <typename T1>
bool gameLoad(const std::string& name, T1 &resource)
{
	for (int i = 0; i < game.getPaths().size(); i++)
	{
		if ( resource.load(game.getPaths()[i] + name) ) return true;
	}
	return false;
}

#endif // _GAME_H_
