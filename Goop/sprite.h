#ifndef SPRITE_H
#define SPRITE_H

#include "gfx.h"
#include <allegro.h>

enum
{
	ALIGN_TOP = 1,
	ALIGN_BOTTOM = 2,
	ALIGN_RIGHT = 4,
	ALIGN_LEFT = 16
};
	
class Sprite
{
	public:
		
	Sprite(BITMAP* bitmap, int xPivot, int yPivot);
	~Sprite();
	
	void draw(BITMAP *where, int x, int y, bool flipped = false, int Alignment = 0);
	void drawBlended(BITMAP *where, int x, int y, int alpha, bool flipped = false, int Alignment = 0, Blenders blender = ADD );
	
	private:
	
	BITMAP *m_bitmap;
	BITMAP *m_mirror;
	int m_xPivot;
	int m_yPivot;
	
};

#endif // _SPRITE_H_
