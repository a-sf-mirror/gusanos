#ifndef PART_TYPE_H
#define PART_TYPE_H

#include "resource_list.h"
#include "part_events.h"

#include <allegro.h>
#include <string>
#include <vector>

class Sprite;

struct TimerEvent
{
	TimerEvent(int _delay, int _delayVariation);
	~TimerEvent();
	PartEvent* event;
	int delay;
	int delayVariation;
};

class PartType
{
	public:
		
	PartType();
	~PartType();
	
	bool load(const std::string &filename);
	
	Sprite* sprite;
	float gravity;
	float damage;
	float bounceFactor;
	int color;
	int repeat;
	int alpha;
	int timeout;
	int timeoutVariation;
	float wormDetectRange;
	float radius;
	int delayBetweenFrames;
	
	std::vector< TimerEvent* > timer;
	PartEvent *groundCollision;
	
};

extern ResourceList<PartType> partTypeList;

#endif // _PART_TYPE_H_
