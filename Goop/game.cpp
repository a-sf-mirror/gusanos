#include "game.h"

#include "part_type.h"
#include "weapon_type.h"
#include "base_object.h"
#include "player.h"
#include "player_input.h"
#include "player_options.h"
#include "level.h"
#include "gconsole.h"
#include "game_actions.h"
#include "base_player.h"
#include "gfx.h"
#include "sfx.h"

#include <allegro.h>
#include <string>
#include <list>

using namespace std;

Game game;

void Options::registerInConsole()
{
	console.registerFloatVariable("SV_NINJAROPE_SHOOT_SPEED", &ninja_rope_shootSpeed, 2);
	console.registerFloatVariable("SV_NINJAROPE_PULL_FORCE", &ninja_rope_pullForce, 0.031);
	console.registerFloatVariable("SV_NINJAROPE_START_DISTANCE", &ninja_rope_startDistance, 20);
	
	console.registerFloatVariable("SV_WORM_MAX_SPEED", &worm_maxSpeed, 0.45);
	console.registerFloatVariable("SV_WORM_ACCELERATION", &worm_acceleration, 0.03);
	console.registerFloatVariable("SV_WORM_FRICTION", &worm_friction, 0.02);
	console.registerFloatVariable("SV_WORM_AIR_FRICTION", &worm_airFriction, 0.000005);
	console.registerFloatVariable("SV_WORM_GRAVITY", &worm_gravity, 0.009);
	console.registerFloatVariable("SV_WORM_BOUNCE_QUOTIENT", &worm_bounceQuotient, 0.3);
	console.registerFloatVariable("SV_WORM_BOUNCE_LIMIT", &worm_bounceLimit, 2);
	console.registerFloatVariable("SV_WORM_JUMP_FORCE", &worm_jumpForce, 0.6);
	console.registerIntVariable("SV_WORM_WEAPON_HEIGHT", &worm_weaponHeight, 4);
	console.registerIntVariable("SV_WORM_HEIGHT", &worm_height, 7);
	console.registerIntVariable("SV_WORM_MAX_CLIMB", &worm_maxClimb, 4);
}

Game::Game()
{
}

Game::~Game()
{
	NRPartType = NULL;
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

	gfx.init();
	sfx.init();
	console.init();
	
	for ( int i = 0; i< MAX_LOCAL_PLAYERS; ++i)
	{
		PlayerOptions *options = new PlayerOptions;
		options->registerInConsole(i);
		playerOptions.push_back(options);
	}
	
	options.registerInConsole();
	gfx.registerInConsole();
	registerGameActions();
	registerPlayerInput();
	
	loadWeapons();
	NRPartType = partTypeList.load("ninjarope.obj");
	
}

void Game::loadWeapons()
{
	string path;
	struct al_ffblk *file=new struct al_ffblk;
	
	path = m_paths[0];
	path += "weapons/*.wpn";
	if ( al_findfirst( (m_paths[0] + "weapons/*.wpn").c_str(), file, FA_ARCH ) == 0 )
	{
		do
		{
			WeaponType* weapon = new WeaponType;
			weapon->load(m_paths[0] + "weapons/" + file->name);
			weaponList.push_back(weapon);
		}
		while(al_findnext(file)==0);
	}
	al_findclose(file);
	
	delete file;
};

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
