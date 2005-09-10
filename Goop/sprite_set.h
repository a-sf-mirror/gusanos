#ifndef SPRITE_SET_H
#define SPRITE_SET_H

#include "resource_list.h"
#include "angle.h"
#include <allegro.h>
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

class Sprite;
	
class SpriteSet
{
public:
		
	SpriteSet();
	~SpriteSet();

	bool load(fs::path const& filename);
	
	Sprite* getSprite( int frame, Angle angle );
	Sprite* getSprite( int frame = 0 );

	inline int getFramesWidth()
	{
		return m_frame[0].size();
	}
			
private:
		
	std::vector< std::vector< Sprite* > > m_frame;
	long m_angleFactor;
	long m_halfAngleDivisonSize;
};

extern ResourceList<SpriteSet> spriteList;

#endif // _SPRITE_SET_H_
