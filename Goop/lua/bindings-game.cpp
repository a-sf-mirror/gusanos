#include "bindings-game.h"

#include "luaapi/types.h"

#include "../glua.h"
#include "../game.h"
#include "../base_player.h"
#include "../player.h"
#include "../base_worm.h"
#include "../level.h"

#include <cmath>
#include <iostream>
#include <list>
#include <allegro.h>
using std::cerr;
using std::endl;
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
using boost::lexical_cast;

namespace LuaBindings
{
	
LuaReference playerIterator(0);
LuaReference playerMetaTable(0);

/*! game_players()

	Returns an iterator object that returns a Player class
	for every player in the game.
	
	Intended to use together
	with a for loop, like this:
	<code>
	for p in game_players() do
	    -- Do something with p here
	end
	</code>
*/
int l_game_players(lua_State* L)
{
	lua.pushReference(LuaBindings::playerIterator);
	typedef std::list<BasePlayer*>::iterator iter;
	iter& i = *(iter *)lua_newuserdata (L, sizeof(iter));
	i = game.players.begin();
	lua_pushnil(L);
	
	return 3;
}

/*! game_local_player(i)

	Returns a Player object of the local player with index i.
	If the index is invalid, nil is returned.
*/
int l_game_localPlayer(lua_State* L)
{
	int i = (int)lua_tointeger(L, 1);
	if(i >= 0 && i < game.localPlayers.size())
	{
		lua.pushReference(game.localPlayers[i]->luaReference);
		return 1;
	}
	else
		return 0;
}


/*! Player:kills()

	Returns the number of kills a player has made.
*/
LMETHOD(BasePlayer, player_kills,
	context.push(p->stats->kills);
	return 1;
)


/*! Player:deaths()

	Returns the number of deaths a player has suffered.
*/
LMETHOD(BasePlayer, player_deaths,
	context.push(p->stats->deaths);
	return 1;
)

/*! Player:name()

	Returns the name of the player.
*/
LMETHOD(BasePlayer, player_name,
	context.push(p->m_name.c_str());
	return 1;
)

/*! Player:team()

	Returns the team number of the player.
*/
LMETHOD(BasePlayer, player_team,
	context.push(p->team);
	return 1;
)

/*! Player:data()

	Returns a lua table associated with this player.
*/
LMETHOD(BasePlayer, player_data,
	if(p->luaData)
		context.pushReference(p->luaData);
	else
	{
		context
			.newtable()
			.pushvalue(-1);
		p->luaData = context.createReference();
	}
	
	return 1;
)

/*! Player:say(text)

	Makes the player send 'text' as a chat message.
*/
LMETHOD(BasePlayer, player_say,
	char const* s = context.tostring(2);
	if(s)
		p->sendChatMsg(s);
	return 0;
)

/*! Player:select_weapons(weapons)

	Tries to change the player's weapons to the WeaponType objects
	in the array //weapons//.
*/
LMETHOD(BasePlayer, player_selectWeapons,
	
	std::vector<WeaponType *> weapons;
	for(size_t i = 1;; ++i)
	{
		context.rawgeti(2, i);
		if(lua_isnil(context, -1))
		{
			break;
		}
		else
		{
			//TODO: Check if this is really a weapon type!
			WeaponType* weapon = *static_cast<WeaponType **>(lua_touserdata(context, -1));
			weapons.push_back(weapon);
			context.pop(); // Pop value
		}
	}
	
	p->selectWeapons(weapons);
	
	return 0;
)


int l_game_getClosestWorm(lua_State* L)
{
	Vec from((float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2));
	
	BaseWorm* minWorm = 0;
	float minDistSqr = 10000000.f;
	
	for(std::list<BasePlayer*>::iterator playerIter = game.players.begin(); playerIter != game.players.end(); ++playerIter)
	{
		BaseWorm* worm = (*playerIter)->getWorm();
		
		if(worm->isActive())
		{
			float distSqr = (worm->pos - from).lengthSqr();
			if(distSqr < minDistSqr)
			{
				minDistSqr = distSqr;
				minWorm = worm;
			}
		}
	}
	
	if(!minWorm)
		return 0;
		
	minWorm->pushLuaReference();
	return 1;
}

int l_game_playerIterator(lua_State* L)
{
	typedef std::list<BasePlayer*>::iterator iter;
	iter& i = *(iter *)lua_touserdata(L, 1);
	if(i == game.players.end())
		lua_pushnil(L);
	else
	{
		lua.pushReference((*i)->luaReference);
		++i;
	}
	
	return 1;
}


/*! map_is_blocked(x1, y1, x2, y2)

	Returns true if the line between (x1, y1) and (x2, y2) on the map is blocked for particles.
	Otherwise false.
*/
int l_map_isBlocked(lua_State* L)
{
	int x1 = lua_tointeger(L, 1);
	int y1 = lua_tointeger(L, 2);
	int x2 = lua_tointeger(L, 3);
	int y2 = lua_tointeger(L, 4);
	lua_pushboolean(L, game.level.trace(x1, y1, x2, y2, Level::ParticleBlockPredicate()));
	return 1;
}

/*! map_is_particle_pass(x1, y1)

	Returns true if the point (x1, y1) on the map is passable by particles.
*/

int l_map_isParticlePass(lua_State* L)
{
	int x = lua_tointeger(L, 1);
	int y = lua_tointeger(L, 2);

	lua_pushboolean(L, game.level.getMaterial(x, y).particle_pass);
	return 1;
}

void initGame()
{
	LuaContext& context = lua;
	
	context.function("game_players", l_game_players);
	lua_pushcfunction(context, l_game_playerIterator);
	playerIterator = context.createReference();
	
	context.function("game_local_player", l_game_localPlayer);
	context.function("game_get_closest_worm", l_game_getClosestWorm);

	context.function("map_is_blocked", l_map_isBlocked);
	context.function("map_is_particle_pass", l_map_isParticlePass);

	// Player method and metatable
	
	CLASS(player,
		("kills", l_player_kills)
		("deaths", l_player_deaths)
		("name", l_player_name)
		("team", l_player_team)
		("say", l_player_say)
		("data", l_player_data)
		("select_weapons", l_player_selectWeapons)
	)
	
/*
	lua_newtable(context); 
	lua_pushstring(context, "__index");
	
	lua_newtable(context);
	
	context.tableFunction("kills", l_player_kills);
	context.tableFunction("deaths", l_player_deaths);
	context.tableFunction("name", l_player_name);
	context.tableFunction("say", l_player_say);
	
	lua_rawset(context, -3);
	playerMetaTable = context.createReference();*/
}

}
