#include "animators.h"

#include "base_animator.h"
#include "sprite_set.h"

#include <allegro.h>

AnimPingPong::AnimPingPong( SpriteSet* sprite, int duration )
: BaseAnimator(0)
{
	//m_totalFrames = sprite->getFramesWidth();
	//m_duration = duration;
	m_max = sprite->getFramesWidth() << 8;
	m_step = m_max / duration;
	m_animPos = 0;
	m_currentDir = 1;
}

/*
AnimPingPong::~AnimPingPong()
{
}*/

/*
int AnimPingPong::getFrame() const
{
	return (m_animPos * m_totalFrames) / m_duration;
}
*/
void AnimPingPong::tick()
{
	if ( freezeTicks <= 0 )
	{
		if (m_currentDir == 1)
		{
			/*
			m_animPos++;
			if( m_animPos >= m_duration )
			{
				m_currentDir = -1;
				m_animPos = m_duration - 1;
			}*/
			
			m_animPos += m_step;
			if(m_animPos >= m_max)
			{
				m_animPos = 2*m_max - m_animPos;
				m_currentDir = -1;
			}
		}
		else
		{
			/*
			m_animPos--;
			if ( m_animPos <= 0 )
			{
				m_currentDir = 1;
				m_animPos = 0;
			}*/
			
			m_animPos -= m_step;
			if(m_animPos < 0)
			{
				m_animPos = -m_animPos;
				m_currentDir = 1;
			}
		}
		m_frame = m_animPos >> 8;
	}else
	{
		--freezeTicks;
	}
}

void AnimPingPong::reset()
{
	m_animPos = 0;
	m_frame = 0;
	m_currentDir = 1;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

AnimLoopRight::AnimLoopRight( SpriteSet* sprite, int duration )
: BaseAnimator(0)
{
	//m_totalFrames = sprite->getFramesWidth();
	//m_duration = duration;
	m_max = sprite->getFramesWidth() << 8;
	m_step = m_max / duration;
	m_animPos = 0;
}
/*
AnimLoopRight::~AnimLoopRight()
{
}*/

/*
int AnimLoopRight::getFrame() const
{
	//return (m_animPos * m_totalFrames) / m_duration;
	return m_frame;
}*/

void AnimLoopRight::tick()
{
	if ( freezeTicks <= 0)
	{
		
		// m_step will never be > m_max since duration can't be < 1
		m_animPos += m_step;
		if(m_animPos >= m_max)
			m_animPos -= m_max;
		m_frame = m_animPos >> 8;
		
		/*
		m_animPos++;
		if( m_animPos >= m_duration )
		{
			m_animPos = 0;
		}*/
	}else
	{
		--freezeTicks;
	}
}

void AnimLoopRight::reset()
{
	m_animPos = 0;
	m_frame = 0;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

AnimRightOnce::AnimRightOnce( SpriteSet* sprite, int duration )
: BaseAnimator(0)
{
	//m_totalFrames = sprite->getFramesWidth();
	//m_duration = duration;
	m_max = (sprite->getFramesWidth() - 1) << 8;
	m_step = m_max / duration;
	m_animPos = 0;
}
/*
AnimRightOnce::~AnimRightOnce()
{
}
*/
/*
int AnimRightOnce::getFrame() const
{
	return (m_animPos * m_totalFrames) / m_duration;
}*/

void AnimRightOnce::tick()
{
	if ( freezeTicks <= 0)
	{
		if( m_animPos < m_max )
		{
			m_animPos += m_step;
			m_frame = m_animPos >> 8;
		}
	}else
	{
		--freezeTicks;
	}
}

void AnimRightOnce::reset()
{
	m_animPos = 0;
	m_frame = 0;
}
