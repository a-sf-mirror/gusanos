#include "game.h"

#include "base_object.h"
#include "level.h"
#include "gconsole.h"

#include <allegro.h>
#include <string>
#include <list>

using namespace std;

Game game;

Game::Game()
{
}

Game::~Game()
{
	list<BaseObject*>::iterator iter;
	for ( iter = objects.begin(); iter != objects.end(); iter++)
	{
		delete *iter;
	}
}

void Game::init()
{
	allegro_init();
	
	game.addPath("default/");
	
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0);

	if(set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
		set_display_switch_mode(SWITCH_BACKGROUND);

	console.init();

}

const vector<string> &Game::getPaths()
{
	return m_paths;
} 

void Game::addPath(const string& path)
{
	m_paths.insert( m_paths.begin(), path);
}

void Game::removePath()
{
	m_paths.erase( m_paths.begin() );
}
