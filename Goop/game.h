#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "base_object.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <list>

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
	
	std::list<BaseObject*> objects;
	Options options;
	
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
