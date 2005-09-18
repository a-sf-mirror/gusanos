#ifndef EXP_TYPE_H
#define EXP_TYPE_H

#include "resource_list.h"
#include "gfx.h"
//#include "events.h"
//#include "distortion.h"
#include "blitters/context.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

class SpriteSet;
class Distortion;
class Event;
class DetectEvent;

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
		//Blenders blender;
		BlitterContext::Type blender;
		bool wupixels;
	
		std::vector< DetectEvent* > detectRanges;
		Event *creation;

};

extern ResourceList<ExpType> expTypeList;

#endif // _PART_TYPE_H_
