#include "player_input.h"
#include "game.h"
#include "player.h"
#include "gconsole.h"
#include "text.h"

#include <string>
#include <list>
#include <vector>

using namespace std;

void registerPlayerInput()
{
	for ( int i = 0; i < MAX_LOCAL_PLAYERS; ++i)
	{
		console.registerSpecialCommand("+P" + cast<string>(i) + "_LEFT", i, leftStart);
		console.registerSpecialCommand("-P" + cast<string>(i) + "_LEFT", i, leftStop);
		console.registerSpecialCommand("+P" + cast<string>(i) + "_RIGHT", i, rightStart);
		console.registerSpecialCommand("-P" + cast<string>(i) + "_RIGHT", i, rightStop);
		console.registerSpecialCommand("+P" + cast<string>(i) + "_UP", i, upStart);
		console.registerSpecialCommand("-P" + cast<string>(i) + "_UP", i, upStop);
		console.registerSpecialCommand("+P" + cast<string>(i) + "_DOWN", i, downStart);
		console.registerSpecialCommand("-P" + cast<string>(i) + "_DOWN", i, downStop);
		console.registerSpecialCommand("+P" + cast<string>(i) + "_CHANGE", i, changeStart);
		console.registerSpecialCommand("-P" + cast<string>(i) + "_CHANGE", i, changeStop);
		console.registerSpecialCommand("+P" + cast<string>(i) + "_JUMP", i, jumpStart);
		console.registerSpecialCommand("-P" + cast<string>(i) + "_JUMP", i, jumpStop);
		console.registerSpecialCommand("+P" + cast<string>(i) + "_FIRE", i, fireStart);
		console.registerSpecialCommand("-P" + cast<string>(i) + "_FIRE", i, fireStop);
	}
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



