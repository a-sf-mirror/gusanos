#ifndef ANIMATORS_H
#define ANIMATORS_H

#include "base_animator.h"

class Sprite;

class AnimPingPong : public BaseAnimator
{
	public:

	AnimPingPong( Sprite* sprite, int duration );
	~AnimPingPong();
	
	int getFrame();
	void tick();
	
	private:

	int m_totalFrames;
	int m_animPos;
	int m_duration;
	char m_currentDir;
};

class AnimLoopRight : public BaseAnimator
{
	public:

	AnimLoopRight( Sprite* sprite, int duration );
	~AnimLoopRight();
	
	int getFrame();
	void tick();
	
	private:

	int m_totalFrames;
	int m_animPos;
	int m_duration;
};

#endif // _ANIMATORS_

