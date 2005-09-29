#ifndef SPRITE_SET_H
#define SPRITE_SET_H

#include "resource_list.h"
#include "angle.h"
#include "cache.h"
#include <allegro.h>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

class Sprite;


class SpriteSet
{
public:
		
	SpriteSet();
#ifndef DEDSERV
	SpriteSet(SpriteSet const&, SpriteSet const& mask, int color);
#endif
	~SpriteSet();

	bool load(fs::path const& filename);
	
	void think();
	
	Sprite* getSprite( size_t frame, Angle angle );
	Sprite* getSprite( size_t frame = 0 );
#ifndef DEDSERV
	Sprite* getColoredSprite( size_t frame, SpriteSet* mask, int color, Angle angle = Angle(0.0) );
#endif

	size_t getFramesWidth()
	{
		return frameCount;
	}
			
private:
	Sprite* getSprite_(size_t frame, size_t angleFrame)
	{
		Sprite* s = m_frame.at(angleFrame*frameCount + frame);
		assert(s);
		return s;
	}
	
	typedef std::pair<SpriteSet*, int> ColorKey;
	
	struct ColorSpriteSet
	{
		ColorSpriteSet(SpriteSet& parent_)
		: parent(parent_)
		{
		}
		
		SpriteSet* operator()(ColorKey const& key);
		
		SpriteSet& parent;
	};
	
	struct DeleteSpriteSet
	{
		void operator()(SpriteSet* spriteSet);
	};
	
	std::vector< Sprite* > m_frame;
	size_t frameCount;
	size_t angleCount;
	//std::map<ColorKey, SpriteSet*> m_coloredCache;
	Cache<ColorKey, SpriteSet*, ColorSpriteSet, DeleteSpriteSet> m_coloredCache;
	long m_angleFactor;
	long m_halfAngleDivisonSize;
};

extern ResourceList<SpriteSet> spriteList;

#endif // _SPRITE_SET_H_
