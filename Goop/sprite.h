#ifndef SPRITE_H
#define SPRITE_H

#include "resource_list.h"

#include <allegro.h>
#include <string>
#include <vector>

class Sprite
{
	public:
		
	Sprite();
	~Sprite();
	
	bool load(const std::string &filename);
	void draw(BITMAP *where, int frame,int x, int y);
	
	private:
	
	std::vector<BITMAP*> m_frame;
	
};

extern ResourceList<Sprite> spriteList;

#endif // _SPRITE_H_
