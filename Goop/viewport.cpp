#include "viewport.h"

#include "game.h"
#include "sfx.h"
#include <list>
#include <allegro.h>

using namespace std;

Viewport::Viewport()
{
	m_dest = NULL;
}

Viewport::~Viewport()
{
	if ( m_dest ) destroy_bitmap(m_dest);
	sfx.freeListener(m_listener);
}

void Viewport::setDestination(BITMAP* where, int x, int y, int width, int height)
{

	if ( m_dest ) destroy_bitmap(m_dest);
	if ( x < 0 ) x = 0;
	if ( y < 0 ) y = 0;
	if ( x + width > where->w ) x = where->w - x;
	if ( y + height > where->h ) y = where->h - y;
	m_dest = create_sub_bitmap(where,x,y,width,height);
	
	m_listener = sfx.newListener();
}

void Viewport::render()
{
	game.level.draw(m_dest,static_cast<int>(m_pos.x), static_cast<int>(m_pos.y));
	
	for ( int i = 0; i < RENDER_LAYERS_AMMOUNT ; ++i)
	{
		ObjectsList::RenderLayerIterator iter;
		for ( iter = game.objects.renderLayerBegin(i); (bool)iter; ++iter)
		{
			(*iter)->draw(m_dest, static_cast<int>(m_pos.x), static_cast<int>(m_pos.y) );
		}
	}
}

void Viewport::setPos(float x, float y)
{
	m_pos.x=x;
	m_pos.y=y;
	
	if ( m_pos.x + m_dest->w > game.level.width() ) m_pos.x = game.level.width() - m_dest->w;
	else if ( m_pos.x < 0 ) m_pos.x = 0;
	if ( m_pos.y + m_dest->h > game.level.height() ) m_pos.y = game.level.height() - m_dest->h;
	else if ( m_pos.y < 0 ) m_pos.y = 0;
		
	if (m_listener) m_listener->pos = m_pos + Vec(m_dest->w/2,m_dest->h/2);
	
}

void Viewport::interpolateTo(float x, float y, float factor)
{
	Vec dest(x-m_dest->w/2,y-m_dest->h/2);

	m_pos = m_pos + (dest-m_pos)*factor;
	
	if ( m_pos.x + m_dest->w > game.level.width() ) m_pos.x = game.level.width() - m_dest->w;
	else if ( m_pos.x < 0 ) m_pos.x = 0;
	if ( m_pos.y + m_dest->h > game.level.height() ) m_pos.y = game.level.height() - m_dest->h;
	else if ( m_pos.y < 0 ) m_pos.y = 0;

	if (m_listener) m_listener->pos = m_pos + Vec(m_dest->w/2,m_dest->h/2);
}

void Viewport::interpolateTo(Vec dest, float factor)
{
	m_pos = m_pos + (dest-Vec(m_dest->w/2,m_dest->h/2)-m_pos)*factor;
	
	if ( m_pos.x + m_dest->w > game.level.width() ) m_pos.x = game.level.width() - m_dest->w;
	else if ( m_pos.x < 0 ) m_pos.x = 0;
	if ( m_pos.y + m_dest->h > game.level.height() ) m_pos.y = game.level.height() - m_dest->h;
	else if ( m_pos.y < 0 ) m_pos.y = 0;
		
	if (m_listener) m_listener->pos = m_pos + Vec(m_dest->w/2,m_dest->h/2);
}
