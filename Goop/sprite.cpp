#include "sprite.h"

#include "resource_list.h"

#include <allegro.h>
#include <string>
#include <vector>

using namespace std;

ResourceList<Sprite> spriteList("sprites/");

Sprite::Sprite()
{
	
}

Sprite::~Sprite()
{
	vector< vector<BITMAP*> >::iterator frameY;
	vector< BITMAP* >::iterator frameX;
	for (frameY = m_frame.begin(); frameY != m_frame.end(); frameY++)
	for (frameX = (*frameY).begin(); frameX != (*frameY).end(); frameX++);
	{
		delete *frameX;
	}
}

bool Sprite::load(const string &filename)
{	
	bool returnValue = false;
	
	BITMAP *tempBitmap = load_bmp(filename.c_str(),0);
	
	if (tempBitmap)
	{
		if ( (tempBitmap->w > 1) && (tempBitmap->h > 1) )
		{
			int lastY = 1;
			
			for (int y = 1; y < tempBitmap->h; ++y)
			{
				
				int lastX = 1;
				
				if(getpixel(tempBitmap,0,y) == 0 || y == tempBitmap->h - 1 )
				{
					m_frame.push_back( vector<BITMAP*>() );
					for (int x = 1; x < tempBitmap->w; ++x)
					{
						if(getpixel(tempBitmap,x,0) == 0 || x == tempBitmap->w - 1 )
						{
							BITMAP* spriteFrame = create_bitmap(x-lastX+1,y-lastY+1);
							blit(tempBitmap,spriteFrame,lastX,lastY,0,0,spriteFrame->w,spriteFrame->h);
							m_frame.back().push_back(spriteFrame);
							lastX = x + 1;
						}
					}
					
					lastY = y + 1;
				}
			}
			
			returnValue = true;
		}
		destroy_bitmap(tempBitmap);
	}
	return returnValue;
}

void Sprite::draw(BITMAP *where, int frame,int x, int y, float angle, bool flipped, int xAligment, int yAligment)
{
	if ( frame < m_frame[0].size() )
	{
		int _x,_y;
		
		float angleDivisionSize = 180 / m_frame.size();
		int angleFrame = (angle + angleDivisionSize / 2 ) * (m_frame.size()-1) / 180;
		
		if ( xAligment == ALIGN_LEFT ) _x = x;
		else if ( xAligment == ALIGN_RIGHT ) _x = x - m_frame[angleFrame][frame]->w;
		else _x = x - m_frame[angleFrame][frame]->w / 2;
			
		if ( yAligment == ALIGN_TOP ) _y = y;
		else if ( yAligment == ALIGN_BOTTOM ) _y = y - m_frame[angleFrame][frame]->h;
		else _y = y - m_frame[angleFrame][frame]->h / 2;
		
		if ( flipped )
			draw_sprite_h_flip(where, m_frame[angleFrame][frame], _x, _y);
		else
			draw_sprite(where, m_frame[angleFrame][frame], _x, _y);
		
	}
}

