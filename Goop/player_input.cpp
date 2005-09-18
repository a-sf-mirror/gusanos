#include "player_input.h"
#include "game.h"
#include "player.h"
#include "gconsole.h"
#include "text.h"
#include <boost/bind.hpp>

#include <string>
#include <list>
#include <vector>

using namespace std;

void registerPlayerInput()
{
	for ( int i = 0; i < Game::MAX_LOCAL_PLAYERS; ++i)
	{
		console.registerCommands()
			("+P" + cast<string>(i) + "_LEFT", boost::bind(leftStart, i, _1))
			("-P" + cast<string>(i) + "_LEFT", boost::bind(leftStop, i, _1))
			("+P" + cast<string>(i) + "_RIGHT", boost::bind(rightStart, i, _1))
			("-P" + cast<string>(i) + "_RIGHT", boost::bind(rightStop, i, _1))
			("+P" + cast<string>(i) + "_UP", boost::bind(upStart, i, _1))
			("-P" + cast<string>(i) + "_UP", boost::bind(upStop, i, _1))
			("+P" + cast<string>(i) + "_DOWN", boost::bind(downStart, i, _1))
			("-P" + cast<string>(i) + "_DOWN", boost::bind(downStop, i, _1))
			("+P" + cast<string>(i) + "_CHANGE", boost::bind(changeStart, i, _1))
			("-P" + cast<string>(i) + "_CHANGE", boost::bind(changeStop, i, _1))
			("+P" + cast<string>(i) + "_JUMP", boost::bind(jumpStart, i, _1))
			("-P" + cast<string>(i) + "_JUMP", boost::bind(jumpStop, i, _1))
			("+P" + cast<string>(i) + "_FIRE", boost::bind(fireStart, i, _1))
			("-P" + cast<string>(i) + "_FIRE", boost::bind(fireStop, i, _1))
		;
	}
	console.registerCommands()
		(string("SAY"), say);
}

string say( const list<string> &args )
{
	if ( !game.localPlayers.empty() )
	{
		if ( !args.empty() )
		{
			game.localPlayers[0]->sendChatMsg( *(args.begin()) );
		}else
		{
			return "SAY <MESSAGE> : SENDS A MESSAGE TO THE OTHER PLAYERS ON THE SERVER";
		}
	}
	return "";
}

string leftStart(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStart(Player::LEFT);
	}
	return "";
}

string leftStop(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStop(Player::LEFT);
	}
	return "";
}

string rightStart(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStart(Player::RIGHT);
	}
	return "";
}

string rightStop(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStop(Player::RIGHT);
	}
	return "";
}

string upStart(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStart(Player::UP);
	}
	return "";
}

string upStop(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStop(Player::UP);
	}
	return "";
}

string downStart(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStart(Player::DOWN);
	}
	return "";
}

string downStop(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStop(Player::DOWN);
	}
	return "";
}

string fireStart(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStart(Player::FIRE);
	}
	return "";
}

string fireStop(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStop(Player::FIRE);
	}
	return "";
}

string jumpStart(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStart(Player::JUMP);
	}
	return "";
}

string jumpStop(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStop(Player::JUMP);
	}
	return "";
}

string changeStart(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStart(Player::CHANGE);
	}
	return "";
}

string changeStop(int index, const list<string> &args)
{
	if ( index < static_cast<int>(game.localPlayers.size()) )
	{
		game.localPlayers[index]->actionStop(Player::CHANGE);
	}
	return "";
}



