#ifndef PART_TYPE_H
#define PART_TYPE_H

#include "resource_list.h"
#include "events.h"
#include "distortion.h"

#include <allegro.h>
#include <string>
#include <vector>

class Sprite;
//struct Distortion;

struct TimerEvent
{
	TimerEvent(int _delay, int _delayVariation);
	~TimerEvent();
	Event* event;
	int delay;
	int delayVariation;
};

struct WormDetectEvent
{
	WormDetectEvent( float range );
	~WormDetectEvent();
	Event* event;
	float m_range;
};

class PartType
{
	public:
		
	PartType();
	~PartType();
	
	bool load(const std::string &filename);

	float gravity;
	float damage;
	float bounceFactor;
	float damping;
	float acceleration;
	float maxSpeed;
	int colour;
	int repeat;
	int alpha;
	int timeout;
	int timeoutVariation;
	float wormDetectRange;
	float radius;
	float angularFriction;
	
	Distortion* distortion;
	float distortMagnitude;
	
	Sprite* sprite;
	int animDuration;
	int animType;
	
	std::vector< TimerEvent* > timer;
	std::vector< WormDetectEvent* > detectRanges;
	Event *groundCollision;
	Event *creation;
	
	enum
	{
		ANIM_PINGPONG = 0,
		ANIM_LOOPRIGHT
	};
	
};

extern ResourceList<PartType> partTypeList;

#endif // _PART_TYPE_H_
