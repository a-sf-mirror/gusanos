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
#include "player_ai.h"
#include "network.h"

#include <allegro.h>
#include <string>
#include <algorithm>
#include <list>

using namespace std;

Game game;

string mapCmd(const list<string> &args)
{
	if (!args.empty())
	{
		string tmp = *args.begin();
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), (int(*)(int)) tolower);
		game.changeLevel( tmp );
		return "";
	}
	return "MAP <MAPNAME> : LOAD A MAP";
}

string gameCmd(const list<string> &args)
{
	if (!args.empty())
	{
		string tmp = *args.begin();
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), (int(*)(int)) tolower);
		game.setMod( tmp );
		return "THE GAME WILL CHANGE THE NEXT TIME YOU CHANGE MAP";
	}
	return "GAME <MODNAME> : SET THE MOD TO LOAD THE NEXT MAP CHANGE";
}

string addbotCmd(const list<string> &args)
{
	game.addBot();
	return "";
}

string connectCmd(const list<string> &args)
{
	if ( !args.empty() )
	{
		network.connect( *args.begin() );
		return "";
	}
	return "CONNECT <HOST_ADDRESS> : JOIN A NETWORK SERVER";
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
	
	console.registerIntVariable("HOST", &host, 0);
	
	console.registerCommand("MAP", mapCmd);
	console.registerCommand("GAME", gameCmd);
	console.registerCommand("ADDBOT", addbotCmd);
	console.registerCommand("CONNECT",connectCmd);
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

	m_defaultPath = "default/";
	m_modPath = "default/";
	nextMod = "default";

	gfx.init();
	sfx.init();
	network.init();
	console.init();
	
	for ( int i = 0; i< MAX_LOCAL_PLAYERS; ++i)
	{
		PlayerOptions *options = new PlayerOptions;
		options->registerInConsole(i);
		playerOptions.push_back(options);
	}
	
	options.registerInConsole();
	gfx.registerInConsole();
	sfx.registerInConsole();
	network.registerInConsole();
	registerGameActions();
	registerPlayerInput();

}

void Game::loadWeapons()
{
	string path;
	struct al_ffblk *file=new struct al_ffblk;
	
	path = m_modPath;
	path += "weapons/*.wpn";
	if ( al_findfirst( path.c_str(), file, FA_ARCH ) == 0 )
	{
		do
		{
			WeaponType* weapon = new WeaponType;
			weapon->load(m_modPath + "weapons/" + file->name);
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
	
	sfx.clear();
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
	unload();
	m_modName = nextMod;
	m_modPath = nextMod + "/";
	if ( !level.load( m_modPath +"maps/"+ levelName ) )
	{
		level.load( m_defaultPath +"maps/"+ levelName );
	}
	level.setName(levelName);
	loadMod();
	
	if ( options.host )
	{
		network.host();
	}
	
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
	{
		nextMod = modname;
	}
	else nextMod = m_modName;
}

const string& Game::getMod()
{
	return m_modName;
}

void Game::addBot()
{
	if ( loaded && level.isLoaded() )
	{
		Worm* worm = new Worm;
		PlayerAI* player = new PlayerAI();
		player->assignWorm(worm);
		objects.push_back( worm );
		objects.push_back( (BaseObject*)worm->getNinjaRopeObj() );
		players.push_back( player );
	}
}
