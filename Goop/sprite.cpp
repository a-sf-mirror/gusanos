#include "sprite.h"

#include "resource_list.h"

#include <allegro.h>
#include <string>
#include <vector>

using namespace std;

ResourceList<Sprite> spriteList;

Sprite::Sprite()
{
	
}

Sprite::~Sprite()
{
	vector<BITMAP*>::iterator frame = m_frame.begin();
	while (frame != m_frame.end())
	{
		delete *frame;
		++frame;
	}
}

bool Sprite::load(const string &filename)
{
	
	BITMAP *tempBitmap = load_bmp(filename.c_str(),0);
	if (tempBitmap && (tempBitmap->h > 1) )
	{
		int lastX = 0;
		for (int x = 0; x < tempBitmap->w; ++x)
		{
			if(getpixel(tempBitmap,x,0) == 0)
			{
				BITMAP* spriteFrame = create_bitmap(x-lastX,tempBitmap->h-1);
				blit(tempBitmap,spriteFrame,lastX,1,0,0,spriteFrame->w,spriteFrame->h);
				m_frame.push_back(spriteFrame); 
				lastX = x + 1;
			}
		}
		BITMAP* spriteFrame = create_bitmap(tempBitmap->w-lastX,tempBitmap->h-1);
		blit(tempBitmap,spriteFrame,lastX,1,0,0,spriteFrame->w,spriteFrame->h);
		m_frame.push_back(spriteFrame);
		
		return true;
	}
	return false;
}

void Sprite::draw(BITMAP *where, int frame,int x, int y, bool flipped, int xAligment, int yAligment)
{
	if ( frame < m_frame.size() )
	{
		int _x,_y;
		if ( xAligment == ALIGN_LEFT ) _x = x;
		else if ( xAligment == ALIGN_RIGHT ) _x = x - m_frame[frame]->w;
		else _x = x - m_frame[frame]->w / 2;
			
		if ( yAligment == ALIGN_TOP ) _y = y;
		else if ( yAligment == ALIGN_BOTTOM ) _y = y - m_frame[frame]->h;
		else _y = y - m_frame[frame]->h / 2;
		
		if ( flipped )
			draw_sprite_h_flip(where, m_frame[frame], _x, _y);
		else
			draw_sprite(where, m_frame[frame], _x, _y);
		
	}
}

