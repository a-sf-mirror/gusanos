#ifndef PARTICLE_H
#define PARTICLE_H

#include "base_object.h"
#include "part_type.h"
#include "util/vec.h"
#include "util/angle.h"
#include "timer_event.h"
#include <vector>
#include <zoidcom.h>

#ifndef DEDSERV
class Sprite;
class BaseAnimator;
class BlitterContext;
class Viewport;
#endif
class BasePlayer;

class Particle : public BaseObject
{
public:

	static ZCom_ClassID  classID;
		
	Particle(PartType* type, Vec pos_ = Vec(0.f, 0.f), Vec spd_ = Vec(0.f, 0.f), int dir = 1, BasePlayer* owner = NULL, Angle angle = Angle(0));
	~Particle();

	void assignNetworkRole( bool authority );
#ifndef DEDSERV
	void draw(Viewport* viewport);
#endif
	void think();
	Angle getAngle();
	void setAngle(Angle v) { m_angle = v; }
	void addAngleSpeed(AngleDiff);
#ifndef DEDSERV
	void setAlphaFade(int frames, int dest);
#endif
	void customEvent( size_t index );
	virtual void pushLuaReference();
	void damage(float amount, BasePlayer* damager );
	void remove();
	virtual void deleteThis();
	
	int getDir() { return m_dir; }
	
	PartType* getType()
	{
		return m_type;
	}
	
	void* operator new(size_t count);
	
	void operator delete(void* block);
	
private:

#ifndef DEDSERV
	void drawLine2Origin(Viewport* viewport, BlitterContext const& blitter);
#endif
	
	int m_dir;
	std::vector< TimerEvent::State > timer; // This could cause a penalty
	PartType* m_type;
	float m_health;
	Angle m_angle;
	AngleDiff m_angleSpeed;
#ifndef DEDSERV
	float m_fadeSpeed;
	float m_alpha;
	int m_alphaDest;
	SpriteSet* m_sprite;
	BaseAnimator* m_animator;
#endif
	Vec m_origin;
	
	LuaReference luaReference;
	ZCom_Node *m_node;
};

#endif  // _PARTICLE_H_
