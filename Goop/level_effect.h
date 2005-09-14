#ifndef LEVEL_EFFECT_H
#define LEVEL_EFFECT_H

#include "resource_list.h"
#include "gfx.h"

#include <allegro.h>
#include <string>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

class SpriteSet;

class LevelEffect
{
	public:
		
		LevelEffect();
		~LevelEffect();
	
		bool load(fs::path const& filename);
		
		SpriteSet* mask;

};

extern ResourceList<LevelEffect> levelEffectList;

#endif // _LEVEL_EFFECT_H_
