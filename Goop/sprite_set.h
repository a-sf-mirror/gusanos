#ifndef SPRITE_SET_H
#define SPRITE_SET_H

#include "resource_list.h"

#include <allegro.h>
#include <string>
#include <vector>

class Sprite;
	
class SpriteSet
{
public:
		
	SpriteSet();
	~SpriteSet();

	bool load(const std::string &filename);
	
	Sprite* getSprite( int frame, float angle );
	Sprite* getSprite( int frame = 0 );

	inline int getFramesWidth()
	{
		return m_frame[0].size();
	}
			
private:
		
	std::vector< std::vector< Sprite* > > m_frame;
};

extern ResourceList<SpriteSet> spriteList;

#endif // _SPRITE_SET_H_
