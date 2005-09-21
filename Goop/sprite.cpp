#include "sprite.h"

#include "gfx.h"
#ifndef DEDSERV
#include "blitters/context.h"
#endif

#include <allegro.h>
#include <iostream>

using namespace std;

Sprite::Sprite( BITMAP* bitmap, int xPivot, int yPivot) : m_bitmap(bitmap)
{
	if ( xPivot == -1 ) m_xPivot = m_bitmap->w / 2;
	else m_xPivot = xPivot;
	if ( yPivot == -1 ) m_yPivot = m_bitmap->h / 2;
	else m_yPivot = yPivot;
	m_mirror = NULL;
}

Sprite::~Sprite()
{
	destroy_bitmap( m_bitmap );
	destroy_bitmap( m_mirror );
}
/*
void Sprite::draw(BITMAP *where, int x, int y, bool flipped, int alignment )
{
	int _x,_y;
	
	if ( alignment & ALIGN_LEFT ) _x = 0;
	else if ( alignment & ALIGN_RIGHT ) _x = m_bitmap->w;
	else _x = m_xPivot;
	
	if ( alignment & ALIGN_TOP ) _y = 0;
	else if ( alignment & ALIGN_BOTTOM ) _y = m_bitmap->h;
	else _y = m_yPivot;
	
	if ( flipped )
		draw_sprite_h_flip(where, m_bitmap, x - ( m_bitmap->w - 1 ) + _x, y - _y);
	else
		draw_sprite( where, m_bitmap, x - _x, y - _y);
}
*/

#ifndef DEDSERV

void Sprite::drawCut(BITMAP *where, int x, int y, BlitterContext const& blender, int alignment, int left, int top, int bottom, int right)
{
	int realX = x + left, realY = y + top;
	
	if ( alignment & ALIGN_LEFT ) /* Do nothing */;
	else if ( alignment & ALIGN_RIGHT ) realX += m_bitmap->w;
	else realX += m_xPivot;
	
	if ( alignment & ALIGN_TOP ) /* Do nothing */;
	else if ( alignment & ALIGN_BOTTOM ) realY += m_bitmap->h;
	else realY += m_yPivot;
	
	int x1 = left, y1 = top, w = m_bitmap->w - (left + right), h = m_bitmap->h - (top + bottom);
	//cerr << m_xPivot << ", " << m_yPivot << endl;
	//masked_blit(m_bitmap, where, x1, y1, realX, realY, w, h);
	blender.drawSpriteCut(where, m_bitmap, realX, realY, left, top, right, bottom);

}

void Sprite::draw(BITMAP *where, int x, int y, bool flipped, int alignment)
{
	draw(where, x, y, BlitterContext(), flipped, alignment);
}

void Sprite::draw(BITMAP *where, int x, int y, BlitterContext const& blender, bool flipped, int alignment )
{
	int _x,_y;
	
	if ( alignment & ALIGN_LEFT ) _x = 0;
	else if ( alignment & ALIGN_RIGHT ) _x = m_bitmap->w;
	else _x = m_xPivot;
	
	if ( alignment & ALIGN_TOP ) _y = 0;
	else if ( alignment & ALIGN_BOTTOM ) _y = m_bitmap->h;
	else _y = m_yPivot;
	
	if ( flipped )
	{
		if(!m_mirror)
		{
			LocalSetColorConversion cc(COLORCONV_NONE);
			LocalSetColorDepth cd(bitmap_color_depth(m_bitmap));
			
			m_mirror = create_bitmap(m_bitmap->w,m_bitmap->h);
			clear_to_color(m_mirror,makecol(255,0,255));
			draw_sprite_h_flip(m_mirror, m_bitmap, 0, 0);
		}

		blender.drawSprite(where, m_mirror, x - ( m_bitmap->w - 1 ) + _x, y - _y);
	}
	else
	{
		blender.drawSprite(where, m_bitmap, x - _x, y - _y);
	}
}
#endif
