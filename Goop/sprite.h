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
	
	private:
	
	std::vector<BITMAP*> m_frame;
	
};

extern ResourceList<Sprite> spriteList;

#endif // _SPRITE_H_
