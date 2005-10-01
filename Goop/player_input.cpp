#ifndef DEDSERV
#include "player_input.h"

#include "game.h"
#include "player.h"
#include "gconsole.h"
#include "glua.h"
#include "text.h"
#include <boost/bind.hpp>

#include <string>
#include <list>
#include <vector>

using namespace std;

string eventStart(size_t index, Player::Actions action, list<string> const& args)
{
	if ( index < game.localPlayers.size() )
	{
		Player& player = *game.localPlayers[index];
		
		EACH_CALLBACK(i, localplayerEvent[action])
		{
			lua.callReference(*i, player.luaReference, true);
		}
		
		player.actionStart(action);
	}
	return "";
}

string eventStop(size_t index, Player::Actions action, list<string> const& args)
{
	if ( index < game.localPlayers.size() )
	{
		Player& player = *game.localPlayers[index];
		
		EACH_CALLBACK(i, localplayerEvent[action])
		{
			lua.callReference(*i, player.luaReference, false);
		}
		
		player.actionStop(action);
	}
	return "";
}

void registerPlayerInput()
{
	for ( size_t i = 0; i < Game::MAX_LOCAL_PLAYERS; ++i)
	{
		static char const* actionNames[] =
		{
			"_LEFT", "_RIGHT", "_UP", "_DOWN", "_FIRE", "_JUMP", "_CHANGE"
		};
		
		for(int action = Player::LEFT; action < Player::ACTION_COUNT; ++action)
		{
			console.registerCommands()
				("+P" + cast<string>(i) + actionNames[action], boost::bind(eventStart, i, (Player::Actions)action, _1))
				("-P" + cast<string>(i) + actionNames[action], boost::bind(eventStop, i, (Player::Actions)action, _1))
			;
		}
	}
	console.registerCommands()
		("SAY", say);
}

string say( const list<string> &args )
{
	if ( !args.empty() )
	{
		if ( !game.localPlayers.empty() )
		{
			game.localPlayers[0]->sendChatMsg( *(args.begin()) );
		}
	}
	else
	{
		return "SAY <MESSAGE> : SENDS A MESSAGE TO THE OTHER PLAYERS ON THE SERVER";
	}
	return "";
}

#endif