#ifndef PART_TYPE_H
#define PART_TYPE_H

#include "resource_list.h"
#include "gfx.h"
#include "events.h"
#include "distortion.h"
#include "angle.h"
#include "vec.h"
#include "blitters/context.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

class SpriteSet;
class DetectEvent;
class BaseAnimator;
class BasePlayer;
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

class PartType;

typedef void (*NewParticleFunc)(PartType* type, Vec pos_, Vec spd_, int dir, BasePlayer* owner, Angle angle);

class PartType
{
	public:
		
	PartType();
	~PartType();
	
	bool isSimpleParticleType();
	bool load(fs::path const& filename);
	
	BaseAnimator* allocateAnimator();
	NewParticleFunc newParticle;

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
	//Blenders blender;
	BlitterContext::Type blender;
	bool line2Origin;
	bool wupixels;
	
	int simpleParticle_timeout;
	int simpleParticle_timeoutVariation;
	
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
