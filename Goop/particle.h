#ifndef PARTICLE_H
#define PARTICLE_H

#include "base_object.h"
#include "part_type.h"
#include "vec.h"
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
	}
	
	inline void reset()
	{
		count = m_tEvent->delay + (int)(rnd() * m_tEvent->delayVariation);
	}
	
	int count;
	TimerEvent* m_tEvent;
};

class Particle : public BaseObject
{
	public:
		
	Particle(PartType* type, Vec _pos = Vec(0,0), Vec _spd = Vec(0,0), int dir = 1, BasePlayer* owner = NULL);

	void draw(BITMAP* where,int xOff, int yOff);
	void think();
	float getAngle();
	void addAngleSpeed(float);
	
	private:
	
	std::vector< PartTimer > timer;
	PartType* m_type;
	float m_angle;
	float m_angleSpeed;
	Sprite* m_sprite;
	BaseAnimator* m_animator;
	
	bool justCreated;
};

#endif  // _PARTICLE_H_
