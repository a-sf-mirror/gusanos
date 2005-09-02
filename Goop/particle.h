#ifndef PARTICLE_H
#define PARTICLE_H

#include "base_object.h"
#include "part_type.h"
#include "vec.h"
#include "angle.h"
#include <vector>

class Sprite;
class BaseAnimator;
class BasePlayer;

struct PartTimer
{
	inline PartTimer( TimerEvent* tEvent)
	{
		m_tEvent = tEvent;
		count = m_tEvent->delay + (int)(rnd() * m_tEvent->delayVariation);
		triggerCount = 0;
	}
	
	inline bool tick()
	{
		if ( m_tEvent->triggerTimes <= 0 || triggerCount < m_tEvent->triggerTimes )
		{
			--count;
			if ( count < 0 )
			{
				reset();
				++triggerCount;
				return true;
			}
		}
		return false;
	}
	
	inline void reset()
	{
		count = m_tEvent->delay + (int)(rnd() * m_tEvent->delayVariation);
	}
	
	int count;
	int triggerCount;
	TimerEvent* m_tEvent; // This could be got easily from the type
};

class Particle : public BaseObject
{
public:
		
	Particle(PartType* type, Vec _pos = Vec(0,0), Vec _spd = Vec(0,0), int dir = 1, BasePlayer* owner = NULL);

	void draw(BITMAP* where,int xOff, int yOff);
	void think();
	Angle getAngle();
	void setAngle(Angle v) { m_angle = v; }
	void addAngleSpeed(AngleDiff);
	void setAlphaFade(int frames, int dest);
	void customEvent( size_t index );
	virtual void pushLuaReference();
	void damage(float amount, BasePlayer* damager );
	
	PartType* getType()
	{
		return m_type;
	}
	
private:
	
	std::vector< PartTimer > timer; // This could cause a penalty
	PartType* m_type;
	float m_health;
	Angle m_angle;
	AngleDiff m_angleSpeed;
	float m_fadeSpeed;
	float m_alpha;
	int m_alphaDest;
	SpriteSet* m_sprite;
	BaseAnimator* m_animator;
	
	bool justCreated;
};

#endif  // _PARTICLE_H_
