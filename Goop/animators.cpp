#include "animators.h"

#include "base_animator.h"
#include "sprite_set.h"

#include <allegro.h>

AnimPingPong::AnimPingPong( SpriteSet* sprite, int duration ) : BaseAnimator()
{
	m_totalFrames = sprite->getFramesWidth();
	m_duration = duration;
	m_animPos = 0;
	m_currentDir = 1;
}

AnimPingPong::~AnimPingPong()
{
}

int AnimPingPong::getFrame()
{
	return (m_animPos * m_totalFrames) / m_duration;
}

void AnimPingPong::tick()
{
	if ( freezeTicks <= 0 )
	{
		if (m_currentDir == 1)
		{
			m_animPos++;
			if( m_animPos >= m_duration )
			{
				m_currentDir = -1;
				m_animPos = m_duration - 1;
			}
		}
		else
		{
			m_animPos--;
			if ( m_animPos <= 0 )
			{
				m_currentDir = 1;
				m_animPos = 0;
			}
		}
	}else
	{
		--freezeTicks;
	}
}

void AnimPingPong::reset()
{
	m_animPos = 0;
	m_currentDir = 1;
}

AnimLoopRight::AnimLoopRight( SpriteSet* sprite, int duration ) : BaseAnimator()
{
	m_totalFrames = sprite->getFramesWidth();
	m_duration = duration;
	m_animPos = 0;
}

AnimLoopRight::~AnimLoopRight()
{
}

int AnimLoopRight::getFrame()
{
	return (m_animPos * m_totalFrames) / m_duration;
}

void AnimLoopRight::tick()
{
	if ( freezeTicks <= 0)
	{
		m_animPos++;
		if( m_animPos >= m_duration )
		{
			m_animPos = 0;
		}
	}else
	{
		--freezeTicks;
	}
}

void AnimLoopRight::reset()
{
	m_animPos = 0;
}
