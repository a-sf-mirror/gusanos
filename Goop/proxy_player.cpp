#include "proxy_player.h"
#include "player_options.h"

ProxyPlayer::ProxyPlayer() : BasePlayer()
{
	m_options = new PlayerOptions();
}

ProxyPlayer::~ProxyPlayer()
{
	delete m_options;
}

void ProxyPlayer::subThink()
{
}

#ifndef DEDSERV
void ProxyPlayer::render()
{
}
#endif



