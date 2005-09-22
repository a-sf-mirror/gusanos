#include "game.h"

#include "base_worm.h"
#include "worm.h"
#include "part_type.h"
#include "exp_type.h"
#include "weapon_type.h"
#include "base_object.h"
#include "player.h"
#include "player_options.h"
#include "particle.h"
#include "explosion.h"
#include "exp_type.h"
#include "level_effect.h"
#include "level.h"
#include "gconsole.h"
#include "game_actions.h"
#include "base_player.h"
#include "proxy_player.h"
#include "gfx.h"
#include "sprite_set.h"
#ifndef DEDSERV
#include "sfx.h"
#include "sound.h"
#include "font.h"
#include "menu.h"
#include "keyboard.h"
#include "player_input.h"
#include "viewport.h"
#endif //DEDSERV
#include "player_ai.h"
#include "net_worm.h"
#include "network.h"
#include "script.h"
#include "ninjarope.h"

#include "loaders/gusanos.h"
#include "loaders/lierox.h"
#include "loaders/liero.h"
#include "loaders/losp.h"
#include "glua.h"
#include "lua/bindings.h"

#include <allegro.h>
#include <string>
#include <algorithm>
#include <list>
#include <iostream>
#include <sstream> //TEMP

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

//using namespace std; // Conflicting
using std::string;
using std::list;
using std::vector;
using std::cerr;
using std::endl;

ZCom_ClassID Game::classID = ZCom_Invalid_ID;

Game game;

string mapCmd(const list<string> &args)
{
	if (!args.empty())
	{
		string tmp = *args.begin();
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), (int(*)(int)) tolower);
		game.changeLevelCmd( tmp );
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

string rConCmd(const list<string> &args)
{
	if ( !args.empty() && network.isClient() )
	{
		string tmp = *args.begin();
		game.sendRConMsg( tmp );
		return "";
	}
	return "";
}

void Options::registerInConsole()
{
	console.registerVariables()
		("SV_NINJAROPE_SHOOT_SPEED", &ninja_rope_shootSpeed, 2)
		("SV_NINJAROPE_PULL_FORCE", &ninja_rope_pullForce, 0.031)
		//("SV_NINJAROPE_PULL_FORCE", &ninja_rope_pullForce, (4.0f / 3.0f) * (70.0f * 70.0f / (100.0f * 100.0f * 16.0f)))
		
		//("SV_NINJAROPE_START_DISTANCE", &ninja_rope_startDistance, 20)
		("SV_NINJAROPE_START_DISTANCE", &ninja_rope_startDistance, 4000.0f / 16.f - 1.f)

		("SV_WORM_MAX_SPEED", &worm_maxSpeed, 0.45)
		("SV_WORM_ACCELERATION", &worm_acceleration, 0.03)
		("SV_WORM_AIR_ACCELERATION_FACTOR", &worm_airAccelerationFactor, 1)
		//("SV_WORM_FRICTION", &worm_friction, 0.02)
		("SV_WORM_FRICTION", &worm_friction, pow(0.89, 0.7))
		("SV_WORM_AIR_FRICTION", &worm_airFriction, 0.000005)
		("SV_WORM_GRAVITY", &worm_gravity, 0.009)
		("SV_WORM_DISABLE_WALL_HUGGING", &worm_disableWallHugging, 0)
		//("SV_WORM_BOUNCE_QUOTIENT", &worm_bounceQuotient, 0.3)
		("SV_WORM_BOUNCE_QUOTIENT", &worm_bounceQuotient, 0.333)
		("SV_WORM_BOUNCE_LIMIT", &worm_bounceLimit, 2)
		//("SV_WORM_BOUNCE_LIMIT", &worm_bounceLimit, 0.56875f)
		
		("SV_WORM_JUMP_FORCE", &worm_jumpForce, 0.6)
		("SV_WORM_WEAPON_HEIGHT", &worm_weaponHeight, 5)
		("SV_WORM_HEIGHT", &worm_height, 9)
		("SV_WORM_WIDTH", &worm_width, 3)
		("SV_WORM_MAX_CLIMB", &worm_maxClimb, 4)
 		("SV_WORM_BOX_RADIUS", &worm_boxRadius, 2)
		("SV_WORM_BOX_TOP", &worm_boxTop, 3)
		("SV_WORM_BOX_BOTTOM", &worm_boxBottom, 4)

		("SV_MAX_RESPAWN_TIME", &maxRespawnTime, -1 )
		("SV_MIN_RESPAWN_TIME", &minRespawnTime, 100 )

		("HOST", &host, 0)
			
		("RCON_PASSWORD", &rConPassword, "" )
	;
	
	console.registerCommands()
		(string("MAP"), mapCmd)
		(string("GAME"), gameCmd)
		(string("ADDBOT"), addbotCmd)
		(string("CONNECT"),connectCmd)
		(string("RCON"),rConCmd)
	;
}

Game::Game()
{
	NRPartType = NULL;
	deathObject = NULL;
	loaded = false;
	m_node = NULL;
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
	
#ifndef DEDSERV
	fontLocator.registerLoader(&GusanosFontLoader::instance);
	fontLocator.registerLoader(&LOSPFontLoader::instance);
	fontLocator.registerLoader(&LieroFontLoader::instance);

	xmlLocator.registerLoader(&XMLLoader::instance);
	gssLocator.registerLoader(&GSSLoader::instance);
#endif

	scriptLocator.registerLoader(&LuaLoader::instance);
	
	LuaBindings::init();

	m_defaultPath = "default/";
	m_modPath = "default/";
	m_modName = "default";
	setMod("default");
	refreshResources();

#ifndef DEDSERV
	keyHandler.init();
#endif
	console.init();
#ifndef DEDSERV
	OmfgGUI::menu.init();
	
	sfx.registerInConsole();
#endif
	gfx.registerInConsole();
	options.registerInConsole();
#ifndef DISABLE_ZOIDCOM
	network.registerInConsole();
#endif
	
	for ( int i = 0; i< MAX_LOCAL_PLAYERS; ++i)
	{
		PlayerOptions *options = new PlayerOptions;
		options->registerInConsole(i);
		playerOptions.push_back(options);
	}
	
#ifndef DEDSERV
	console.executeConfig("config.cfg");
#else
	console.executeConfig("config-ded.cfg");
#endif

	parseCommandLine(argc, argv);
	
	gfx.init();
#ifndef DEDSERV
	sfx.init();
#endif

#ifndef DISABLE_ZOIDCOM
	network.init();
#endif
	registerGameActions();
#ifndef DEDSERV
	registerPlayerInput();
#endif
}

inline void addEvent(ZCom_BitStream* data, int event)
{
	data->addInt( event, 8 );
}

void Game::think()
{
	if ( !m_node )
		return;
	
	while ( m_node->checkEventWaiting() )
	{
		ZCom_Node::eEvent type;
		eZCom_NodeRole    remote_role;
		ZCom_ConnID       conn_id;
		
		ZCom_BitStream *data = m_node->getNextEvent(&type, &remote_role, &conn_id);
		switch(type)
		{
			case ZCom_Node::eEvent_User:
			if ( data )
			{
				NetEvents event = (NetEvents)data->getInt(8);
				switch ( event )
				{
					case eHole:
					{
						int index = data->getInt(8);
						int x = data->getInt(32);
						int y = data->getInt(32);
						level.applyEffect( levelEffectList[index], x, y );
					}
					break;
				}
			}
			break;
			
			case ZCom_Node::eEvent_Init:
			{
				list<LevelEffectEvent>::iterator iter = appliedLevelEffects.begin();
				for( ; iter != appliedLevelEffects.end() ; ++iter )
				{
					ZCom_BitStream *data = new ZCom_BitStream;
					addEvent(data, eHole);
					data->addInt( iter->index, 8);
					data->addInt( iter->x, 32 );
					data->addInt( iter->y, 32 );
					m_node->sendEventDirect(eZCom_ReliableOrdered, data, conn_id );
				}
			}
			break;
			
			default: break; // Annoying warnings >:O
		}
	}
}

void Game::applyLevelEffect( LevelEffect* effect, int x, int y )
{
	if ( !network.isClient() )
	{
		if ( level.applyEffect( effect, x, y ) && network.isHost() )
		{
			ZCom_BitStream *data = new ZCom_BitStream;
			addEvent(data, eHole);
			data->addInt(effect->getIndex(), 8); // TODO: Gliptic can optimize this if he wants ;O
			data->addInt( x, 32 ); // this too ;OOO
			data->addInt( y, 32 );
			m_node->sendEvent(eZCom_ReliableOrdered, ZCOM_REPRULE_AUTH_2_ALL, data);
			
			appliedLevelEffects.push_back( LevelEffectEvent(effect->getIndex(), x, y ) );
		}
	}
}

void Game::loadWeapons()
{
	fs::path path( m_modPath );
	path /= "weapons";
	
	if ( fs::exists( path ) )
	{
		fs::directory_iterator end_itr;
		
		for( fs::directory_iterator iter(path); iter != end_itr; ++iter)
		{
			if( !is_directory(*iter) )
			{
				if ( fs::extension(*iter) == ".wpn")
				{
					WeaponType* weapon = new WeaponType;
					weapon->load(*iter);
					weaponList.push_back(weapon);
				}
			}
		}
		
		WeaponOrder comp;
		std::sort(weaponList.begin(), weaponList.end(), comp); 
	}
};

void Game::loadMod()
{
	console.loadResources();
	loadWeapons();
	NRPartType = partTypeList.load("ninjarope.obj");
	deathObject = partTypeList.load("death.obj");
	digObject = partTypeList.load("wormdig.obj");
#ifndef DEDSERV
	infoFont = fontLocator.load("minifont");
#endif
	if (weaponList.size() > 0 )
	{
		loaded = true;
		levelEffectList.indexate();
	}
	else
	{
		loaded = false;
		console.addLogMsg("ERROR: NO WEAPONS FOUND IN MOD FOLDER");
	}
	console.executeConfig("mod.cfg");
}

void Game::unload()
{
	//cerr << "Unloading..." << endl;
	loaded = false;
#ifndef DEDSERV
	OmfgGUI::menu.clear();
	sfx.clear();
#endif
	
	console.clearTemporaries();
	
	// Delete all objects
#ifdef USE_GRID
	objects.clear();
#else
	for ( ObjectsList::Iterator iter = objects.begin(); (bool)iter; ++iter)
	{
		delete (*iter);
	}
	objects.clear();
#endif

	appliedLevelEffects.clear();
	
	// Delete all players
	for ( list<BasePlayer*>::iterator iter = players.begin(); iter != players.end(); ++iter)
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
	expTypeList.clear();
#ifndef DEDSERV
	soundList.clear();
#endif
	spriteList.clear();
	levelEffectList.clear();

#ifndef DEDSERV	
	fontLocator.clear();
	xmlLocator.clear();
	gssLocator.clear();
#endif
	scriptLocator.clear();
	
	lua.reset();
	luaCallbacks = LuaCallbacks(); // Reset callbacks
}

bool Game::isLoaded()
{
	return loaded;
}

void Game::refreshResources()
{
#ifndef DEDSERV
	fontLocator.addPath(fs::path("default/fonts"));
	fontLocator.addPath(fs::path(nextMod) / "fonts");
	fontLocator.refresh();

	xmlLocator.addPath(fs::path("default/gui"));
	xmlLocator.addPath(fs::path(nextMod) / "gui");
	xmlLocator.refresh();
	
	gssLocator.addPath(fs::path("default/gui"));
	gssLocator.addPath(fs::path(nextMod) / "gui");
	gssLocator.refresh();
#endif
	
	scriptLocator.addPath(fs::path("default/scripts"));
	scriptLocator.addPath(fs::path(nextMod) / "scripts");
	scriptLocator.refresh();
	
	// These are added in reverse order compared to
	// the resource locator paths! Fix maybe?
	partTypeList.addPath(fs::path(level.getPath()) / "objects");
	partTypeList.addPath(fs::path(nextMod) / "objects");
	partTypeList.addPath(fs::path("default/objects"));
	
	expTypeList.addPath(fs::path(level.getPath()) / "objects");
	expTypeList.addPath(fs::path(nextMod) / "objects");
	expTypeList.addPath(fs::path("default/objects"));
	
#ifndef DEDSERV
	soundList.addPath(fs::path(level.getPath()) / "sounds");
	soundList.addPath(fs::path(nextMod) / "sounds");
	soundList.addPath(fs::path("default/sounds"));
#endif
	
	spriteList.addPath(fs::path(level.getPath()) / "sprites");
	spriteList.addPath(fs::path(nextMod) / "sprites");
	spriteList.addPath(fs::path("default/sprites"));
	
	levelEffectList.addPath(fs::path(level.getPath()) / "mapeffects");
	levelEffectList.addPath(fs::path(nextMod) / "mapeffects");
	levelEffectList.addPath(fs::path("default/mapeffects"));
}

void Game::changeLevelCmd(const std::string& levelName )
{
	changeLevel( levelName );
	
	network.disconnect();
	if ( options.host && !network.isClient() )
	{
#ifndef DISABLE_ZOIDCOM
		network.host();
#endif
	}
	
	//cerr << "Creating players and worms" << endl;
	// All this is temporal, dont be scared ;D
	if ( loaded && level.isLoaded() ) 
	{
		if ( network.isHost() )
		{
			if(true)
			{
				// TODO: Factorize all this out, its being duplicated on client.cpp also :O
				BaseWorm* worm = addWorm(true); 
				BasePlayer* player = addPlayer ( OWNER );
				player->changeName( playerOptions[0]->name );
				player->assignNetworkRole(true);
				player->assignWorm(worm);
			}
		}else if ( !network.isClient() )
		{
			if(true)
			{
				BaseWorm* worm = addWorm(true);
				BasePlayer* player = addPlayer ( OWNER );
				player->changeName( playerOptions[0]->name );
				player->assignWorm(worm);
			}
		}
	}
	//cerr << "changeLevel() done." << endl;
}

void Game::changeLevel(const std::string& levelName )
{
	//cerr << "Unloading resources" << endl;
	unload();
	LuaBindings::init();
	
	m_modName = nextMod;
	m_modPath = nextMod + "/";

	level.setName(levelName);
	refreshResources();
	//cerr << "Loading level" << endl;
	levelLocator.clear();
	levelLocator.addPath(fs::path("default/maps"));
	levelLocator.addPath(fs::path(nextMod) / "maps");
	levelLocator.refresh();
	
	levelLocator.load(&level, levelName);
#ifdef USE_GRID
	objects.resize(0, 0, level.width(), level.height());
#endif
	//cerr << "Loading mod" << endl;
	loadMod();
	
}

void Game::assignNetworkRole( bool authority )
{
	m_node = new ZCom_Node;
	if (!m_node)
	{
		allegro_message("ERROR: Unable to create game node.");
	}

	m_node->beginReplicationSetup(0);
		//m_node->addReplicationInt( (zS32*)&deaths, 32, false, ZCOM_REPFLAG_MOSTRECENT, ZCOM_REPRULE_AUTH_2_ALL , 0);
	m_node->addReplicationInt( (zS32*)&options.worm_gravity, 32, false, ZCOM_REPFLAG_MOSTRECENT | ZCOM_REPFLAG_RARELYCHANGED, ZCOM_REPRULE_AUTH_2_ALL );
	
	m_node->endReplicationSetup();

	m_isAuthority = authority;
	if( authority)
	{
		m_node->setEventNotification(true, false); // Enables the eEvent_Init.
		if( !m_node->registerNodeUnique(classID, eZCom_RoleAuthority, network.getZControl() ) )
			allegro_message("ERROR: Unable to register player authority node.");
	}else
	{
		if( !m_node->registerNodeUnique( classID, eZCom_RoleProxy, network.getZControl() ) )
			allegro_message("ERROR: Unable to register player requested node.");
	}

	m_node->applyForZoidLevel(1);
}

void Game::sendRConMsg( string const& message )
{
	ZCom_BitStream *req = new ZCom_BitStream;
	req->addInt(Network::RConMsg, 8);
	req->addString( options.rConPassword.c_str() );
	req->addString( message.c_str() );
	network.getZControl()->ZCom_sendData( network.getServerID(), req, eZCom_ReliableOrdered );
}

void Game::removeNode()
{
	delete m_node;
	m_node = NULL;
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

void Game::displayChatMsg( std::string const& owner, std::string const& message)
{
	console.addLogMsg( "<" + owner + "> " + message );
}

const string& Game::getMod()
{
	return m_modName;
}

const string& Game::getModPath()
{
	return m_modPath;
}

const string& Game::getDefaultPath()
{
	return m_defaultPath;
}

BasePlayer* Game::findPlayerWithID( ZCom_NodeID ID )
{
	list<BasePlayer*>::iterator playerIter;
	for ( playerIter = game.players.begin(); playerIter != game.players.end(); ++playerIter)
	{
		if ( (*playerIter)->getNodeID() == ID )
		{
			return (*playerIter);
		}
	}
	return NULL;
}

void Game::insertExplosion( Explosion* explosion )
{
#ifdef USE_GRID
	game.objects.insert( explosion, Grid::NoColLayer, explosion->getType()->renderLayer);
#else
	game.objects.insert( NO_COLLISION_LAYER, explosion->getType()->renderLayer, explosion );
#endif
}

BasePlayer* Game::addPlayer( PLAYER_TYPE type )
{
	BasePlayer *retPlayer = NULL;
	if( type == OWNER )
	{
		Player* player = new Player(playerOptions[0]);
#ifndef DEDSERV
		Viewport* viewport = new Viewport;
		viewport->setDestination(gfx.buffer,0,0,320,240);
		player->assignViewport(viewport);
#endif
		players.push_back( player );
		localPlayers.push_back( player );
		retPlayer = player;
	}else if ( type == PROXY )
	{
		ProxyPlayer* player = new ProxyPlayer();
		players.push_back( player );
		retPlayer = player;
	}else if ( type == AI )
	{
		PlayerAI* player = new PlayerAI();
		players.push_back( player );
		player->changeName( "bot" );
		retPlayer = player;
	}
	return retPlayer;
}

BaseWorm* Game::addWorm(bool isAuthority)
{
	BaseWorm* returnWorm = NULL;
	if ( network.isHost() || network.isClient() )
	{
		NetWorm* netWorm = new NetWorm(isAuthority);
		returnWorm = netWorm;
	}else
	{
		Worm* worm = new Worm();
		returnWorm = worm;
	}
#ifdef USE_GRID
	objects.insertImmediately(returnWorm, Grid::WormColLayer, Grid::WormRenderLayer);
	objects.insertImmediately(returnWorm->getNinjaRopeObj(), 1, 1);
#else
	objects.insert(WORMS_COLLISION_LAYER,WORMS_RENDER_LAYER, returnWorm);
	objects.insert( 1,1, (BaseObject*)returnWorm->getNinjaRopeObj() );
#endif

	return returnWorm;
}

void Game::addBot()
{
	if ( loaded && level.isLoaded() )
	{
		BaseWorm* worm = addWorm(true); 
		BasePlayer* player = addPlayer(AI);
		if ( network.isHost() ) player->assignNetworkRole(true);
		player->assignWorm(worm);
	}
}

unsigned long Game::stringToIndex(std::string const& str)
{
	HashTable<std::string, unsigned long>::iterator i = stringToIndexMap.find(str);
	if(i != stringToIndexMap.end())
		return i->second;
	i = stringToIndexMap.insert(str);
	int idx = indexToStringMap.size();
	i->second = idx;
	indexToStringMap.push_back(str);
	return idx;
}

std::string const& Game::indexToString(unsigned long idx)
{
	return indexToStringMap.at(idx);
}