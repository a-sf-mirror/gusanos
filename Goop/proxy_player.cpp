#include "proxy_player.h"
#include "player_options.h"
#include <allegro.h>

using namespace std;

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

void ProxyPlayer::render()
{
}



