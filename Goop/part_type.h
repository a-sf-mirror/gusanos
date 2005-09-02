#ifndef PART_TYPE_H
#define PART_TYPE_H

#include "resource_list.h"
#include "gfx.h"
#include "events.h"
#include "distortion.h"
#include "angle.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

class SpriteSet;
class DetectEvent;
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

class PartType
{
	public:
		
	PartType();
	~PartType();
	
	bool load(fs::path const& filename);

	float gravity;
	float bounceFactor;
	float groundFriction;
	float damping;
	float acceleration;
	float maxSpeed;
	int repeat;
	AngleDiff angularFriction;
	int colLayer;
	float health;
	
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
	std::vector< DetectEvent* > detectRanges;
	std::vector< Event* > customEvents;
	Event *groundCollision;
	Event *creation;
	Event *death;
	
	enum
	{
		ANIM_PINGPONG = 0,
		ANIM_LOOPRIGHT,
		ANIM_RIGHTONCE
	};
	
};

extern ResourceList<PartType> partTypeList;

#endif // _PART_TYPE_H_
