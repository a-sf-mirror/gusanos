#include "game.h"

#include "base_object.h"
#include "level.h"
#include "gconsole.h"
#include "game_actions.h"

#include <fmod/fmod.h>

#include <allegro.h>
#include <string>
#include <list>

using namespace std;

Game game;

void Options::registerInConsole()
{
	console.registerFloatVariable("SV_WORM_MAX_SPEED", &worm_maxSpeed, 1);
	console.registerFloatVariable("SV_WORM_ACCELERATION", &worm_acceleration, 0.1);
	console.registerFloatVariable("SV_WORM_FRICTION", &worm_friction, 0.04);
	console.registerFloatVariable("SV_WORM_AIR_FRICTION", &worm_airFriction, 0.005);
	console.registerFloatVariable("SV_WORM_GRAVITY", &worm_gravity, 0.04);
	console.registerFloatVariable("SV_WORM_BOUNCE_QUOTIENT", &worm_bounceQuotient, 0.3);
	console.registerFloatVariable("SV_WORM_BOUNCE_LIMIT", &worm_bounceLimit, 4);
	console.registerFloatVariable("SV_WORM_JUMP_FORCE", &worm_jumpForce, 1.2);
	console.registerIntVariable("SV_WORM_WEAPON_HEIGHT", &worm_weaponHeight, 4);
	console.registerIntVariable("SV_WORM_HEIGHT", &worm_height, 7);
	console.registerIntVariable("SV_WORM_MAX_CLIMB", &worm_maxClimb, 4);
}

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
	
	FSOUND_Init(44100, 32, 0);
	game.addPath("default/");
	
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0);

	if(set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
		set_display_switch_mode(SWITCH_BACKGROUND);

	console.init();
	options.registerInConsole();
	
	registerGameActions();
	
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
