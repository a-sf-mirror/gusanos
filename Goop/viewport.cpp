#include "viewport.h"

#include "game.h"
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
}

void Viewport::setDestination(BITMAP* where, int x, int y, int width, int height)
{
	
	if ( m_dest ) destroy_bitmap(m_dest);
	if ( x < 0 ) x = 0;
	if ( y < 0 ) y = 0;
	m_dest = create_sub_bitmap(where,x,y,width,height);
	
}

void Viewport::render()
{
	game.level.draw(m_dest,(int)m_pos.x,(int)m_pos.y);
	
	list<BaseObject*>::iterator iter;
	for ( iter = game.objects.begin(); iter != game.objects.end(); iter++)
	{
		(*iter)->draw(m_dest, m_pos.x, m_pos.y);
	}
}

void Viewport::setPos(float x, float y)
{
	m_pos.x=x;
	m_pos.y=y;
}

void Viewport::interpolateTo(float x, float y, float factor)
{
	Vec dest(x,y);

	m_pos = m_pos + (dest-m_pos)*factor;
}
