#include "proxy_player.h"
#include "player_options.h"

ProxyPlayer::ProxyPlayer()
: BasePlayer(shared_ptr<PlayerOptions>(new PlayerOptions))
{

}

ProxyPlayer::~ProxyPlayer()
{

}

void ProxyPlayer::subThink()
{
}

#ifndef DEDSERV
void ProxyPlayer::render()
{
}
#endif



