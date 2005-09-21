#ifndef EXP_TYPE_H
#define EXP_TYPE_H

#include "resource_list.h"
#ifndef DEDSERV
#include "gfx.h"
#include "blitters/context.h"
#endif

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

#ifndef DEDSERV
		Distortion* distortion;
		float distortMagnitude;
		SpriteSet* sprite;
		BlitterContext::Type blender;
#endif //DEDSERV
		int renderLayer;
		int colour;
		int alpha;
		int destAlpha;

		bool wupixels;
	
		std::vector< DetectEvent* > detectRanges;
		Event *creation;

};

extern ResourceList<ExpType> expTypeList;

#endif // _PART_TYPE_H_
