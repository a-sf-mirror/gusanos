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

void BasePlayer::assignWorm(Worm* worm)
{
	m_worm = worm;
	worm->assignOwner( this );
}

PlayerOptions *BasePlayer::getOptions()
{
	return m_options;
}


