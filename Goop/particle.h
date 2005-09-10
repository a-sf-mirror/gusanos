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
		count = m_tEvent->delay + rndInt(m_tEvent->delayVariation);
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
		count = m_tEvent->delay + rndInt(m_tEvent->delayVariation);
	}
	
	int count;
	int triggerCount;
	TimerEvent* m_tEvent; // This could be got easily from the type
};

class Particle : public BaseObject
{
public:
		
	Particle(PartType* type, Vec pos_ = Vec(0.f, 0.f), Vec spd_ = Vec(0.f, 0.f), int dir = 1, BasePlayer* owner = NULL, Angle angle = Angle(0));
	~Particle();

	void draw(BITMAP* where,int xOff, int yOff);
	void think();
	Angle getAngle();
	void setAngle(Angle v) { m_angle = v; }
	void addAngleSpeed(AngleDiff);
	void setAlphaFade(int frames, int dest);
	void customEvent( size_t index );
	virtual void pushLuaReference();
	void damage(float amount, BasePlayer* damager );
	
	int getDir() { return m_dir; }
	
	PartType* getType()
	{
		return m_type;
	}
	
	void* operator new(size_t count);
	
	void operator delete(void* block);
	
private:
	
	void drawLine2Origin(BITMAP* where, int xOff, int yOff);
	
	int m_dir;
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
	Vec m_origin;
	
	
	bool justCreated;
};

#endif  // _PARTICLE_H_
