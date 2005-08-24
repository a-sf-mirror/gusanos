#ifndef PART_TYPE_H
#define PART_TYPE_H

#include "resource_list.h"
#include "gfx.h"
#include "events.h"
#include "distortion.h"

#include <allegro.h>
#include <string>
#include <vector>

class SpriteSet;
//struct Distortion;

struct TimerEvent
{
	TimerEvent(int _delay, int _delayVariation, int _triggerTimes);
	~TimerEvent();
	Event* event;
	int delay;
	int delayVariation;
	int triggerTimes;
};

struct WormDetectEvent
{
	WormDetectEvent( float range, bool detectOwner );
	~WormDetectEvent();
	Event* event;
	float m_range;
	bool m_detectOwner;
};

class PartType
{
	public:
		
	PartType();
	~PartType();
	
	bool load(const std::string &filename);

	float gravity;
	float bounceFactor;
	float groundFriction;
	float damping;
	float acceleration;
	float maxSpeed;
	int repeat;
	float angularFriction;
	
	Distortion* distortion;
	float distortMagnitude;
	
	int renderLayer;
	int colour;
	int alpha;
	SpriteSet* sprite;
	int animDuration;
	int animType;
	int animOnGround;
	Blenders blender;
	
	std::vector< TimerEvent* > timer;
	std::vector< WormDetectEvent* > detectRanges;
	Event *groundCollision;
	Event *creation;
	
	enum
	{
		ANIM_PINGPONG = 0,
		ANIM_LOOPRIGHT,
		ANIM_RIGHTONCE
	};
	
};

extern ResourceList<PartType> partTypeList;

#endif // _PART_TYPE_H_
