#include "sprite.h"

#include "resource_list.h"
#include "gfx.h"

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
	for (frameX = (*frameY).begin(); frameX != (*frameY).end(); frameX++)
	{
		if (*frameX) destroy_bitmap( *frameX );
	}
}

bool Sprite::load(const string &filename)
{	
	bool returnValue = false;
	
	BITMAP *tempBitmap = gfx.loadBitmap(filename.c_str(),0);
	
	if (tempBitmap)
	{
		if ( (tempBitmap->w > 1) && (tempBitmap->h > 1) )
		{
			int lastY = 1;
			int pivotY = -1;
			
			for (int y = 1; y < tempBitmap->h; ++y)
			{
				
				if( getpixel(tempBitmap,0,y) == makecol(255,0,0) ) // Red pixel marks the pivot of the sprite
				{
					pivotY = y-lastY;
				}
				else	if(getpixel(tempBitmap,0,y) == 0 || y == tempBitmap->h - 1 )
				{
					m_frame.push_back( vector<BITMAP*>() );

					int lastX = 1;
					int pivotX = -1;
					
					for (int x = 1; x < tempBitmap->w; ++x)
					{
						if( getpixel(tempBitmap,x,0) == makecol(255,0,0) ) // Pivot again but for X axis
						{
							pivotX = x-lastX;
						}
						else if(getpixel(tempBitmap,x,0) == 0 || x == tempBitmap->w - 1 )
						{
							BITMAP* spriteFrame = create_bitmap(x-lastX+1,y-lastY+1);
							blit(tempBitmap,spriteFrame,lastX,lastY,0,0,spriteFrame->w,spriteFrame->h);
							m_frame.back().push_back(spriteFrame);
							
							m_pivotX.push_back( pivotX );
							pivotX = -1;
							
							lastX = x + 1;
						}
					}

					m_pivotY.push_back( pivotY );
					pivotY = -1;
					lastY = y + 1;
				}
			}
			
			returnValue = true;
		}
		destroy_bitmap(tempBitmap);
	}
	return returnValue;
}

void Sprite::draw(BITMAP *where, int frame,int x, int y, bool flipped, int xAligment, int yAligment)
{
	if ( frame < m_frame[0].size() )
	{
		int _x,_y;
		
		if ( xAligment == ALIGN_LEFT ) _x = 0;
		else if ( xAligment == ALIGN_RIGHT ) _x = m_frame[0][frame]->w;
		else 
		{
			if ( m_pivotX[frame] == -1 )
				_x = m_frame[0][frame]->w / 2;
			else
				_x = m_pivotX[frame];
		}
		
		if ( yAligment == ALIGN_TOP ) _y = 0;
		else if ( yAligment == ALIGN_BOTTOM ) _y = m_frame[0][frame]->h;
		else 
		{
			if ( m_pivotY[0] == -1 )
				_y = m_frame[0][frame]->h / 2;
			else
				_y = m_pivotY[0];
		}
		
		if ( flipped )
			draw_sprite_h_flip(where, m_frame[0][frame], x - (m_frame[0][frame]->w - 1) + _x, y - _y);
		else
			draw_sprite(where, m_frame[0][frame], x - _x, y - _y);
	
	}
}

void Sprite::drawAngled(BITMAP *where, int frame,int x, int y, float angle, bool flipped, int xAligment, int yAligment)
{
	if ( frame < m_frame[0].size() )
	{
		int _x,_y;
		
		float angleDivisionSize = 180 / m_frame.size();
		int angleFrame = (angle + angleDivisionSize / 2 ) * (m_frame.size()-1) / 180;
		
		if ( angleFrame < m_frame.size() )
		{
		
			if ( xAligment == ALIGN_LEFT ) _x = 0;
			else if ( xAligment == ALIGN_RIGHT ) _x = m_frame[angleFrame][frame]->w;
			else 
			{
				if ( m_pivotX[frame] == -1 )
					_x = m_frame[angleFrame][frame]->w / 2;
				else
					_x = m_pivotX[frame];
			}
			
			if ( yAligment == ALIGN_TOP ) _y = 0;
			else if ( yAligment == ALIGN_BOTTOM ) _y = m_frame[angleFrame][frame]->h;
			else 
			{
				if ( m_pivotY[angleFrame] == -1 )
					_y = m_frame[angleFrame][frame]->h / 2;
				else
					_y = m_pivotY[angleFrame];
			}
			
			if ( flipped )
				draw_sprite_h_flip(where, m_frame[angleFrame][frame], x - (m_frame[angleFrame][frame]->w - 1) + _x, y - _y);
			else
				draw_sprite(where, m_frame[angleFrame][frame], x - _x, y - _y);
		}
	}
}
