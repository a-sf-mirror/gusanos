#ifndef EXP_TYPE_H
#define EXP_TYPE_H

#include "resource_list.h"
#include "gfx.h"
#include "events.h"
#include "distortion.h"

#include <allegro.h>
#include <string>
#include <vector>

class SpriteSet;
//struct Distortion;

struct DetectEvent
{
	DetectEvent( float range, bool detectOwner );
	~DetectEvent();
	Event* event;
	float m_range;
	bool m_detectOwner;
};

class ExpType
{
	public:
		
		ExpType();
		~ExpType();
	
		bool load(const std::string &filename);
		
		int timeout;
		int timeoutVariation;

		Distortion* distortion;
		float distortMagnitude;
	
		int renderLayer;
		int colour;
		int alpha;
		int destAlpha;
		SpriteSet* sprite;
		Blenders blender;
	
		std::vector< DetectEvent* > detectRanges;
		Event *creation;

};

extern ResourceList<ExpType> expTypeList;

#endif // _PART_TYPE_H_
