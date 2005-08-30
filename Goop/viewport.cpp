#include "viewport.h"

#include "game.h"
#include "sfx.h"
#include "base_worm.h"
#include "base_player.h"
#include "lua/bindings.h"
//#include "culling.h"
#include <list>
#include <allegro.h>

#include <iostream>

using namespace std;

Viewport::Viewport()
{
	m_dest = NULL;
	
	LuaBindings::pushViewport(this);
	luaReference = game.lua.createReference();
}

Viewport::~Viewport()
{
	game.lua.destroyReference(luaReference);
	destroy_bitmap(m_dest);
	sfx.freeListener(m_listener);
}

struct TestCuller
{
	TestCuller(BITMAP* dest_, int x_, int y_, int scrOffX_, int scrOffY_, int limitX_)
	: dest(dest_), x(x_), y(y_), scrOffX(scrOffX_), scrOffY(scrOffY_), limitX(limitX_)
	{
		
	}
	
	int beginH()
	{
		return y;
	}
	
	int beginV()
	{
		return x;
	}
	
	int limitV()
	{
		return limitX;
	}
	
	void incrementV(int& vp)
	{
		++vp;
	}
	
	bool block(int y, int x)
	{
		return !game.level.getMaterial(x, y).worm_pass;
	}
	
	void line(int x, int y1, int y2)
	{
		vline(dest, x + scrOffX, y1 + scrOffY, y2 + scrOffY, 0);
	}
	
	BITMAP* dest;
	int x;
	int y;
	int scrOffX;
	int scrOffY;
	int limitX;
};

void Viewport::setDestination(BITMAP* where, int x, int y, int width, int height)
{

	destroy_bitmap(m_dest);
	if ( x < 0 ) x = 0;
	if ( y < 0 ) y = 0;
	if ( x + width > where->w ) x = where->w - x;
	if ( y + height > where->h ) y = where->h - y;
	m_dest = create_sub_bitmap(where,x,y,width,height);
	
	m_listener = sfx.newListener();
}

void Viewport::render(BasePlayer* player)
{
	int offX = static_cast<int>(m_pos.x);
	int offY = static_cast<int>(m_pos.y);
	game.level.draw(m_dest, offX, offY);
	
/*
	static bool flag = false;
	
	if(!flag)
	{
		list<BaseObject*>::iterator iter = game.objects.begin();
		//++iter; ++iter;
		int x = (int)(*iter)->getPos().x;
		int y = (int)(*iter)->getPos().y;
		Culler<TestCuller> testCuller(TestCuller(m_dest, x, y, -(int)m_pos.x, -(int)m_pos.y, x + 200));
		testCuller.cull();
		//flag = true;
	}
	*/

	for ( int i = 0; i < RENDER_LAYERS_AMMOUNT ; ++i)
	{
		ObjectsList::RenderLayerIterator iter;
		for ( iter = game.objects.renderLayerBegin(i); (bool)iter; ++iter)
		{
			(*iter)->draw(m_dest, offX, offY);
		}
	}

	EACH_CALLBACK(i, wormRender)
	{
		for(list<BasePlayer*>::iterator playerIter = game.players.begin(); playerIter != game.players.end(); ++playerIter)
		{
			BaseWorm* worm = (*playerIter)->getWorm();
			if(worm->isActive())
			{
				Vec renderPos = worm->getRenderPos();
				int x = (int)renderPos.x - offX;
				int y = (int)renderPos.y - offY;
				game.lua.callReference(*i, (lua_Number)x, (lua_Number)y, worm->luaReference, luaReference);
			}
		}
		
	}
	
	EACH_CALLBACK(i, viewportRender)
	{
		game.lua.callReference(*i, luaReference, player->getWorm()->luaReference);
	}
}

void Viewport::setPos(float x, float y)
{
	m_pos.x=x;
	m_pos.y=y;
	
	if (m_listener) m_listener->pos = m_pos + Vec(m_dest->w/2,m_dest->h/2);
	
	if ( m_pos.x + m_dest->w > game.level.width() ) m_pos.x = game.level.width() - m_dest->w;
	else if ( m_pos.x < 0 ) m_pos.x = 0;
	if ( m_pos.y + m_dest->h > game.level.height() ) m_pos.y = game.level.height() - m_dest->h;
	else if ( m_pos.y < 0 ) m_pos.y = 0;
	
}

void Viewport::interpolateTo(float x, float y, float factor)
{
	Vec dest(x-m_dest->w/2,y-m_dest->h/2);

	m_pos = m_pos + (dest-m_pos)*factor;
	
	if (m_listener) m_listener->pos = Vec(x,y);
	
	if ( m_pos.x + m_dest->w > game.level.width() ) m_pos.x = game.level.width() - m_dest->w;
	else if ( m_pos.x < 0 ) m_pos.x = 0;
	if ( m_pos.y + m_dest->h > game.level.height() ) m_pos.y = game.level.height() - m_dest->h;
	else if ( m_pos.y < 0 ) m_pos.y = 0;
}

void Viewport::interpolateTo(Vec dest, float factor)
{
	m_pos = m_pos + (dest-Vec(m_dest->w/2,m_dest->h/2)-m_pos)*factor;
	
	if (m_listener) m_listener->pos = dest;
	
	if ( m_pos.x + m_dest->w > game.level.width() ) m_pos.x = game.level.width() - m_dest->w;
	else if ( m_pos.x < 0 ) m_pos.x = 0;
	if ( m_pos.y + m_dest->h > game.level.height() ) m_pos.y = game.level.height() - m_dest->h;
	else if ( m_pos.y < 0 ) m_pos.y = 0;
}
