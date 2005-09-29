#include "sprite_set.h"

#include "resource_list.h"
#include "gfx.h"
#include "sprite.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <iostream> //TEMP
#include <stdexcept>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

using namespace std;

ResourceList<SpriteSet> spriteList("sprites/");

SpriteSet::SpriteSet()
: m_coloredCache(ColorSpriteSet(*this))
{
	
}

#ifndef DEDSERV
// This does not copy the colored cache naturally
SpriteSet::SpriteSet(SpriteSet const& b, SpriteSet const& mask, int color)
: m_frame(b.m_frame), m_angleFactor(b.m_angleFactor)
, m_halfAngleDivisonSize(b.m_halfAngleDivisonSize)
, frameCount(b.frameCount)
, angleCount(b.angleCount)
, m_coloredCache(ColorSpriteSet(*this))
{
	std::vector<Sprite *>::const_iterator srci = b.m_frame.begin();
	std::vector<Sprite *>::const_iterator maski = mask.m_frame.begin();
	std::vector<Sprite *>::iterator desti = m_frame.begin();
	for (; desti != m_frame.end();
		++srci, ++maski, ++desti)
	{
		if((*srci)->m_bitmap->w != (*maski)->m_bitmap->w
		|| (*srci)->m_bitmap->h != (*maski)->m_bitmap->h)
			throw std::runtime_error("Mask sprite is not the same size as source sprite");
			
		*desti = new Sprite(**srci, **maski, color);
	}
}
#endif

SpriteSet::~SpriteSet()
{
	std::vector<Sprite *>::iterator frame = m_frame.begin();
	for (; frame != m_frame.end(); frame++)
	{
		delete *frame;
	}
	
/*
	std::map<ColorKey, SpriteSet*>::iterator i = m_coloredCache.begin();
	for(; i != m_coloredCache.end(); ++i)
	{
		delete i->second;
	}*/
}

bool SpriteSet::load(fs::path const& filename)
{	
	//cerr << "Loading sprite set: " << filename.native_file_string() << endl;
	
	BITMAP *tempBitmap = gfx.loadBitmap(filename.native_file_string().c_str(), 0, true);
	
	if (!tempBitmap)
		return false;
		
	LocalSetColorConversion cc(COLORCONV_NONE);
	LocalSetColorDepth cd(bitmap_color_depth(tempBitmap));

	if ( (tempBitmap->w > 1) && (tempBitmap->h > 1) )
	{
		int lastY = 1;
		int pivotY = -1;
		angleCount = 0;
		
		for (int y = 1; y < tempBitmap->h; ++y)
		{
			if( getpixel(tempBitmap,0,y) == makecol(255,0,0) ) // Red pixel marks the pivot of the sprite
			{
				pivotY = y-lastY;
			}
			else if( getpixel(tempBitmap,0,y) == 0 || y == tempBitmap->h - 1 )
			{
				++angleCount;
				
				int lastX = 1;
				int pivotX = -1;
				frameCount = 0;
				
				for (int x = 1; x < tempBitmap->w; ++x)
				{
					if( getpixel(tempBitmap,x,0) == makecol(255,0,0) ) // Pivot again but for X axis
					{
						pivotX = x-lastX;
					}
					else if(getpixel(tempBitmap,x,0) == 0 || x == tempBitmap->w - 1 )
					{
						BITMAP* spriteFrame = create_bitmap(x-lastX+1, y-lastY+1);
						blit(tempBitmap, spriteFrame, lastX, lastY, 0, 0, spriteFrame->w, spriteFrame->h);
						//m_frame.back().push_back(new Sprite( spriteFrame, pivotX, pivotY ) );
						m_frame.push_back(new Sprite( spriteFrame, pivotX, pivotY ) );
						++frameCount;
						
						pivotX = -1;
						
						lastX = x + 1;
					}
				}

				pivotY = -1;
				lastY = y + 1;
			}
		}
			
		// Fill the other 180º with the sprites but mirrored.

	}

	destroy_bitmap(tempBitmap);
	
	m_angleFactor = (angleCount - 1) * 2;
	m_halfAngleDivisonSize = (1 << 15) / angleCount / 2;

	return true;
}

Sprite* SpriteSet::getSprite( size_t frame )
{
	if ( frame > frameCount )
	{
		frame = 0;
	}
	return getSprite_(frame, 0);
}

Sprite* SpriteSet::getSprite( size_t frame, Angle angle )
{
	angle.clamp();
	if ( frame > frameCount )
	{
		frame = 0;
	}

	size_t angleFrame = ((angle.adjust<16>() + m_halfAngleDivisonSize) * m_angleFactor) >> 16;

	if ( angleFrame >= angleCount )
	{
		angleFrame = angleCount - 1;
	}
	return getSprite_(frame, angleFrame);
}

#ifndef DEDSERV
SpriteSet* SpriteSet::ColorSpriteSet::operator()(ColorKey const& key)
{
	cerr << "Coloring sprite set" << endl;
	return new SpriteSet(parent, *key.first, key.second);
}

void SpriteSet::DeleteSpriteSet::operator()(SpriteSet* spriteSet)
{
	cerr << "Deleting sprite set" << endl;
	delete spriteSet;
}

Sprite* SpriteSet::getColoredSprite( size_t frame, SpriteSet* mask, int color, Angle angle )
{
	if(!mask
	|| mask->frameCount != frameCount
	|| mask->angleCount != angleCount)
		return 0;
		
	ColorKey key = std::make_pair(mask, color);
	
	SpriteSet* s = m_coloredCache[key];
	
	return s->getSprite(frame, angle);
	
	/*
	std::map<ColorKey, SpriteSet*>::iterator i
		= m_coloredCache.find(key);
		
	if(i == m_coloredCache.end())
	{
		i = m_coloredCache.insert(
			std::make_pair(key,
				new SpriteSet(*this, *mask, color))).first;
	}

	return i->second->getSprite(frame, angle);*/
}
#endif

