#include "game.h"

#include "viewport.h"
#include "worm.h"
#include "part_type.h"
#include "weapon_type.h"
#include "sound.h"
#include "sprite.h"
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

string mapCmd(const list<string> &args)
{
	if (!args.empty())
	{
		game.changeLevel( *args.begin() );
		return "";
	}
	return "MAP <MAPNAME> : LOAD A MAP";
}

string gameCmd(const list<string> &args)
{
	if (!args.empty())
	{
		game.setMod( *args.begin() );
		return "THE GAME WILL CHANGE THE NEXT TIME YOU CHANGE MAP";
	}
	return "GAME <MODNAME> : SET THE MOD TO LOAD THE NEXT MAP CHANGE";
}

void Options::registerInConsole()
{
	console.registerFloatVariable("SV_NINJAROPE_SHOOT_SPEED", &ninja_rope_shootSpeed, 2);
	console.registerFloatVariable("SV_NINJAROPE_PULL_FORCE", &ninja_rope_pullForce, 0.031);
	console.registerFloatVariable("SV_NINJAROPE_START_DISTANCE", &ninja_rope_startDistance, 20);
	
	console.registerFloatVariable("SV_WORM_MAX_SPEED", &worm_maxSpeed, 0.45);
	console.registerFloatVariable("SV_WORM_ACCELERATION", &worm_acceleration, 0.03);
	console.registerFloatVariable("SV_WORM_AIR_ACCELERATION_FACTOR", &worm_airAccelerationFactor, 1);
	console.registerFloatVariable("SV_WORM_FRICTION", &worm_friction, 0.02);
	console.registerFloatVariable("SV_WORM_AIR_FRICTION", &worm_airFriction, 0.000005);
	console.registerFloatVariable("SV_WORM_GRAVITY", &worm_gravity, 0.009);
	console.registerFloatVariable("SV_WORM_BOUNCE_QUOTIENT", &worm_bounceQuotient, 0.3);
	console.registerFloatVariable("SV_WORM_BOUNCE_LIMIT", &worm_bounceLimit, 2);
	console.registerFloatVariable("SV_WORM_JUMP_FORCE", &worm_jumpForce, 0.6);
	console.registerIntVariable("SV_WORM_WEAPON_HEIGHT", &worm_weaponHeight, 4);
	console.registerIntVariable("SV_WORM_HEIGHT", &worm_height, 7);
	console.registerIntVariable("SV_WORM_MAX_CLIMB", &worm_maxClimb, 4);
	
	console.registerCommand("MAP", mapCmd);
	console.registerCommand("GAME", gameCmd);
}

Game::Game()
{
	NRPartType = NULL;
	loaded = true;
}

Game::~Game()
{

}

void Game::init()
{
	allegro_init();

	defaultPath = "default/";
	modPath = "default/";
	nextMod = "default/";

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
	
}

void Game::loadWeapons()
{
	string path;
	struct al_ffblk *file=new struct al_ffblk;
	
	path = modPath;
	path += "weapons/*.wpn";
	if ( al_findfirst( path.c_str(), file, FA_ARCH ) == 0 )
	{
		do
		{
			WeaponType* weapon = new WeaponType;
			weapon->load(modPath + "weapons/" + file->name);
			weaponList.push_back(weapon);
		}
		while(al_findnext(file)==0);
	}
	al_findclose(file);
	
	delete file;
};

void Game::loadMod()
{
	console.loadResources();
	loadWeapons();
	NRPartType = partTypeList.load("ninjarope.obj");
	if (weaponList.size() > 0 )
		loaded = true;
	else
	{
		loaded = false;
		console.addLogMsg("ERROR: NO WEAPONS FOUND IN MOD FOLDER");
	}
}

void Game::unload()
{
	loaded = false;
	// Delete all objects
	for ( list<BaseObject*>::iterator iter = objects.begin(); iter != objects.end(); ++iter)
	{
		delete (*iter);
	}
	objects.clear();
	
	// Delete all players
	for ( vector<BasePlayer*>::iterator iter = players.begin(); iter != players.end(); ++iter)
	{
		delete (*iter);
	}
	players.clear();
	localPlayers.clear();
	
	level.unload();
	
	for ( vector<WeaponType*>::iterator iter = weaponList.begin(); iter != weaponList.end(); ++iter)
	{
		delete (*iter);
	}
	weaponList.clear();
	
	partTypeList.clear();
	soundList.clear();
	spriteList.clear();
	fontList.clear();
}

bool Game::isLoaded()
{
	return loaded;
}

void Game::changeLevel(const std::string& levelName )
{
	game.unload();
	modPath = nextMod;
	if ( !level.load( modPath +"maps/"+ levelName ) )
	{
		level.load( defaultPath +"maps/"+ levelName );
	}
	game.loadMod();
	
	if ( loaded && level.isLoaded() )
	{
		if(true)
		{
			Worm* worm = new Worm;
			Player* player = new Player(playerOptions[0]);
			Viewport* viewport = new Viewport;
			viewport->setDestination(gfx.buffer,0,0,160,240);
			player->assignWorm(worm);
			player->assignViewport(viewport);
			objects.push_back( worm );
			objects.push_back( (BaseObject*)worm->getNinjaRopeObj() );
			players.push_back( player );
			localPlayers.push_back( player );
		}
		if(true)
		{
			Worm* worm = new Worm;
			Player* player = new Player(playerOptions[1]);
			Viewport* viewport = new Viewport;
			viewport->setDestination(gfx.buffer,160,0,160,240);
			player->assignWorm(worm);
			player->assignViewport(viewport);
			objects.push_back( worm );
			objects.push_back( (BaseObject*)worm->getNinjaRopeObj() );
			players.push_back( player );
			localPlayers.push_back( player );
		}
	}
}

void Game::setMod( const string& modname )
{
	if ( file_exists( modname.c_str(), FA_DIREC, NULL) )
		nextMod = modname + "/";
	else nextMod = defaultPath;
}

