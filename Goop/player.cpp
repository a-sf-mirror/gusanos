#include "player.h"
#include "player_options.h"
#include "worm.h"
#include "viewport.h"

#include <allegro.h>

using namespace std;

Player::Player(PlayerOptions* options) : BasePlayer()
{
	bool aimingUp = false;
	bool aimingDown = false;
	
	m_options = options;

	m_viewport = NULL;
}

Player::~Player()
{
	if ( m_viewport ) delete m_viewport;
}

void Player::assignViewport(Viewport* viewport)
{
	m_viewport = viewport;
}

void Player::think()
{
	if ( m_worm )
	{
		if ( m_viewport ) m_viewport->interpolateTo(m_worm->getPos(),m_options->viewportFollowFactor);
		if ( aimingUp ) m_worm->addAimSpeed(-m_options->aimAcceleration);
		else if ( aimingDown ) m_worm->addAimSpeed(m_options->aimAcceleration);
	}
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
		
		case UP:
		{
			if ( m_worm )
			{
				aimingUp = true;
			}
		}
		break;
		
		case DOWN:
		{
			if ( m_worm )
			{
				aimingDown = true;
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
		
		case UP:
		{
			if ( m_worm )
			{
				aimingUp = false;
			}
		}
		break;
		
		case DOWN:
		{
			if ( m_worm )
			{
				aimingDown = false;
			}
		}
		break;

	}
}




