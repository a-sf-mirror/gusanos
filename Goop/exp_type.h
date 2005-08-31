#ifndef EXP_TYPE_H
#define EXP_TYPE_H

#include "resource_list.h"
#include "gfx.h"
//#include "events.h"
//#include "distortion.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

class SpriteSet;
class Distortion;
class Event;

struct DetectEvent
{
	DetectEvent( float range, bool detectOwner, int detectFilter );
	~DetectEvent();
	Event* event;
	float m_range;
	bool m_detectOwner;
	unsigned int m_detectFilter; // detect filters ored together, 1 is worms filter, 2^n for the custom filters with n > 0
};

class ExpType
{
	public:
		
		ExpType();
		~ExpType();
	
		bool load(fs::path const& filename);
		
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
