#ifndef PARTICLE_H
#define PARTICLE_H

#include "base_object.h"
#include "part_type.h"
#include "vec.h"
#include <vector>

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
		
	Particle();
	Particle(PartType* type);
	Particle(PartType* type, Vec _pos, Vec _spd );

	void draw(BITMAP* where,int xOff, int yOff);
	void think();
	float getAngle();
	
	private:
	
	std::vector< PartTimer > timer;
	PartType* m_type;
	float m_angle;
	float m_angleSpeed;
	
	bool justCreated;
};

#endif  // _PARTICLE_H_
