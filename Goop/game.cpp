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
#include "font.h"
#include "gconsole.h"
#include "game_actions.h"
#include "base_player.h"
#include "gfx.h"
#include "sfx.h"
#include "player_ai.h"
#include "net_worm.h"
#include "network.h"
#include "menu.h"
#include "keyboard.h"

#include "loaders/gusanos.h"
#include "loaders/lierox.h"
#include "loaders/liero.h"
#include "loaders/losp.h"

#include <allegro.h>
#include <string>
#include <algorithm>
#include <list>
#include <iostream>

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
#ifndef DISABLE_ZOIDCOM
		network.connect( *args.begin() );
#endif
		return "";
	}
	return "CONNECT <HOST_ADDRESS> : JOIN A NETWORK SERVER";
}

void Options::registerInConsole()
{
	console.registerVariables()
		("SV_NINJAROPE_SHOOT_SPEED", &ninja_rope_shootSpeed, 2)
		("SV_NINJAROPE_PULL_FORCE", &ninja_rope_pullForce, 0.031)
		("SV_NINJAROPE_START_DISTANCE", &ninja_rope_startDistance, 20)
		
		("SV_WORM_MAX_SPEED", &worm_maxSpeed, 0.45)
		("SV_WORM_ACCELERATION", &worm_acceleration, 0.03)
		("SV_WORM_AIR_ACCELERATION_FACTOR", &worm_airAccelerationFactor, 1)
		("SV_WORM_FRICTION", &worm_friction, 0.02)
		("SV_WORM_AIR_FRICTION", &worm_airFriction, 0.000005)
		("SV_WORM_GRAVITY", &worm_gravity, 0.009)
		("SV_WORM_BOUNCE_QUOTIENT", &worm_bounceQuotient, 0.3)
		("SV_WORM_BOUNCE_LIMIT", &worm_bounceLimit, 2)
		("SV_WORM_JUMP_FORCE", &worm_jumpForce, 0.6)
		("SV_WORM_WEAPON_HEIGHT", &worm_weaponHeight, 4)
		("SV_WORM_HEIGHT", &worm_height, 7)
		("SV_WORM_MAX_CLIMB", &worm_maxClimb, 4)
		
		("HOST", &host, 0)
	;
	
	console.registerCommands()
		("MAP", mapCmd)
		("GAME", gameCmd)
		("ADDBOT", addbotCmd)
		("CONNECT",connectCmd)
	;
}

Game::Game()
{
	NRPartType = NULL;
	loaded = true;
}

Game::~Game()
{

}

void Game::parseCommandLine(int argc, char** argv)
{
	for(int i = 0; i < argc; ++i)
	{
		const char* arg = argv[i];
		if(arg[0] == '-')
		{
			switch(arg[1])
			{
				case 'c':
					if(++i >= argc)
						return;
						
					console.parseLine(argv[i]);
				break;
			}
		}
	}
}

void Game::init(int argc, char** argv)
{
	allegro_init();

	levelLocator.registerLoader(&GusanosLevelLoader::instance);
	levelLocator.registerLoader(&LieroXLevelLoader::instance);
	levelLocator.registerLoader(&LieroLevelLoader::instance);
	
	fontLocator.registerLoader(&GusanosFontLoader::instance);
	fontLocator.registerLoader(&LOSPFontLoader::instance);
	fontLocator.registerLoader(&LieroFontLoader::instance);

	m_defaultPath = "default/";
	m_modPath = "default/";
	setMod("default");
	refreshResources(); // 

	keyHandler.init();
	console.init();
	OmfgGUI::menu.init();
	
	sfx.registerInConsole();
	gfx.registerInConsole();

	parseCommandLine(argc, argv);

	gfx.init();
	sfx.init();
#ifndef DISABLE_ZOIDCOM
	network.init();
#endif

	for ( int i = 0; i< MAX_LOCAL_PLAYERS; ++i)
	{
		PlayerOptions *options = new PlayerOptions;
		options->registerInConsole(i);
		playerOptions.push_back(options);
	}
	
	
	//TODO: Check and move the rest of registerInConsole() before init()
	options.registerInConsole(); 
#ifndef DISABLE_ZOIDCOM
	network.registerInConsole();
#endif
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
	infoFont = fontLocator.load("minifont");
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
	
	OmfgGUI::menu.clear();
	
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
	//fontList.clear();
	
	fontLocator.clear();
}

bool Game::isLoaded()
{
	return loaded;
}

void Game::refreshResources()
{
	fontLocator.addPath(fs::path("default/fonts"));
	fontLocator.addPath(fs::path(nextMod) / "fonts");
	fontLocator.refresh();
}

void Game::changeLevel(const std::string& levelName )
{
	unload();
	
	m_modName = nextMod;
	m_modPath = nextMod + "/";
	/*
	if ( !level.load( m_modPath +"maps/"+ levelName ) )
	{
		level.load( m_defaultPath +"maps/"+ levelName );
	}
	*/
	level.setName(levelName); // Nice GLIPTIC! >;o
	refreshResources();
	levelLocator.load(&level, levelName);
	loadMod();
	
	if ( options.host )
	{
#ifndef DISABLE_ZOIDCOM
		network.host();
#endif
	}
	
	// All this is temporal, dont be scared ;D
	if ( loaded && level.isLoaded() ) 
	{
		if ( network.isHost() )
		{
			if(true)
			{
				NetWorm* worm = new NetWorm(true);
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
				NetWorm* worm = new NetWorm(true);
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
		}else
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

}

void Game::setMod( const string& modname )
{
	if ( file_exists( modname.c_str(), FA_DIREC, NULL) ) //TODO: Change to Boost.Filesystem
	{
		nextMod = modname;
	}
	else nextMod = m_modName;
	
	levelLocator.clear();
	levelLocator.addPath(fs::path("default/maps"));
	levelLocator.addPath(fs::path(nextMod) / "maps");
	levelLocator.refresh();
	
	
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
