#ifndef SPRITE_H
#define SPRITE_H

#include "resource_list.h"

#include <allegro.h>
#include <string>
#include <vector>

enum
{
	ALIGN_CENTER = 0,
	ALIGN_TOP,
	ALIGN_BOTTOM,
	ALIGN_RIGHT,
	ALIGN_LEFT
};
	
class Sprite
{
	public:
		
	Sprite();
	~Sprite();
	
	bool load(const std::string &filename);
	void draw(BITMAP *where, int frame,int x, int y, bool flipped = false, int xAligment = ALIGN_CENTER, int yAligment = ALIGN_CENTER);
	void drawAngled(BITMAP *where, int frame,int x, int y, float angle, bool flipped = false, int xAligment = ALIGN_CENTER, int yAligment = ALIGN_CENTER);
	
	inline int getFramesWidth()
	{
		return m_frame[0].size();
	}
	
	private:
	
	std::vector< std::vector< BITMAP* > > m_frame;
	std::vector< int > m_pivotY;
	std::vector< int > m_pivotX;
	
};

extern ResourceList<Sprite> spriteList;

#endif // _SPRITE_H_
