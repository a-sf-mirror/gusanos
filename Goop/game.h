#ifndef GAME_H
#define GAME_H

#include "level.h"
#include "base_object.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <list>

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
