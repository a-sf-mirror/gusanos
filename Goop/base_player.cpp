#include "base_player.h"
#include "worm.h"
#include "player_options.h"

#include <allegro.h>

BasePlayer::BasePlayer()
{
	deaths = 0;
	kills = 0;
	
	m_worm = NULL;
}

BasePlayer::~BasePlayer()
{
}

void BasePlayer::assignWorm(BaseWorm* worm)
{
	m_worm = worm;
	worm->assignOwner( this );
}

PlayerOptions *BasePlayer::getOptions()
{
	return m_options;
}

Vec BasePlayer::getPos()
{
	return m_worm->getPos(); // WARNING: The player could not own a worm if he is a spectator.
}


