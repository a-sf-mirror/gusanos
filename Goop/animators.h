#ifndef ANIMATORS_H
#define ANIMATORS_H

#include "base_animator.h"

class SpriteSet;

class AnimPingPong : public BaseAnimator
{
	public:

	AnimPingPong( SpriteSet* sprite, int duration );
	~AnimPingPong();
	
	virtual int getFrame();
	virtual void tick();
	virtual void reset();
	
	private:

	int m_totalFrames;
	int m_animPos;
	int m_duration;
	char m_currentDir;
};

class AnimLoopRight : public BaseAnimator
{
	public:

	AnimLoopRight( SpriteSet* sprite, int duration );
	~AnimLoopRight();
	
	virtual int getFrame();
	virtual void tick();
	virtual void reset();
	
	private:

	int m_totalFrames;
	int m_animPos;
	int m_duration;
};

class AnimRightOnce : public BaseAnimator
{
	public:

		AnimRightOnce( SpriteSet* sprite, int duration );
		~AnimRightOnce();
	
		virtual int getFrame();
		virtual void tick();
		virtual void reset();
	
	private:

		int m_totalFrames;
		int m_animPos;
		int m_duration;
};

#endif // _ANIMATORS_

