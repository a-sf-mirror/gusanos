#include "player.h"
#include "worm.h"
#include "viewport.h"

#include <allegro.h>

using namespace std;

Player::Player()
{
	team = 0;
	deaths = 0;
	kills = 0;
	color = 0;
	name = "unnamed";
	
	m_worm = NULL;
	m_viewport = NULL;
}

Player::~Player()
{
	if ( m_viewport ) delete m_viewport;
}

void Player::assignWorm(Worm* worm)
{
	m_worm = worm;
}

void Player::assignViewport(Viewport* viewport)
{
	m_viewport = viewport;
}

void Player::think()
{
	if ( m_worm && m_viewport ) m_viewport->interpolateTo(m_worm->getPos(),0.1);
}

void Player::render()
{
	if ( m_viewport ) m_viewport->render();
}

void Player::actionStart ( Actions action )
{
	switch (action)
	{
		case LEFT:
		{
			if ( m_worm )
			{
				m_worm -> actionStart(Worm::MOVELEFT);
			}
		}
		break;
		
		case RIGHT:
		{
			if ( m_worm )
			{
				m_worm -> actionStart(Worm::MOVERIGHT);
			}
		}
		break;
		
		case FIRE:
		{
			if ( m_worm )
			{
				m_worm -> actionStart(Worm::FIRE);
			}
		}
		break;
		
		case JUMP:
		{
			if ( m_worm )
			{
				m_worm -> actionStart(Worm::JUMP);
			}
		}
		break;
		
	}
}

void Player::actionStop ( Actions action )
{
	switch (action)
	{
		case LEFT:
		{
			if ( m_worm )
			{
				m_worm -> actionStop(Worm::MOVELEFT);
			}
		}
		break;
		
		case RIGHT:
		{
			if ( m_worm )
			{
				m_worm -> actionStop(Worm::MOVERIGHT);
			}
		}
		break;
		
		case FIRE:
		{
			if ( m_worm )
			{
				m_worm -> actionStop(Worm::FIRE);
			}
		}
		break;
		
		case JUMP:
		{
			if ( m_worm )
			{
				m_worm -> actionStop(Worm::JUMP);
			}
		}
		break;
		
	}
}




