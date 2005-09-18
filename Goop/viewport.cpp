#include "viewport.h"

#include "game.h"
#include "sfx.h"
#include "base_worm.h"
#include "base_player.h"
#include "glua.h"
#include "lua/bindings.h"
//#include "culling.h"
#include <list>
#include <allegro.h>

#include <iostream>

using namespace std;

Viewport::Viewport()
{
	m_dest = NULL;
	
	lua.pushFullReference(*this, LuaBindings::viewportMetaTable);
	luaReference = lua.createReference();
}

Viewport::~Viewport()
{
	lua.destroyReference(luaReference);
	destroy_bitmap(m_dest);
	sfx.freeListener(m_listener);
}

struct TestCuller
{
	TestCuller(BITMAP* dest_, int scrOffX_, int scrOffY_)
	: dest(dest_), scrOffX(scrOffX_), scrOffY(scrOffY_)
	{
		
	}
	
	bool block(int x, int y)
	{
		return !game.level.getMaterial(x, y).worm_pass;
	}
	
	int lowerLimitH()
	{
		return 0;
	}
	
	int upperLimitH()
	{
		return 200;
	}
	
	void line(int y, int x1, int x2)
	{
		hline(dest, x1 + scrOffX, y + scrOffY, x2 + scrOffY, 0);
	}
	
	BITMAP* dest;
/*
	int x;
	int y;*/
	int scrOffX;
	int scrOffY;
	//int limitX;
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
		list<BasePlayer*>::iterator iter = game.players.begin();
		//++iter; ++iter;
		BaseWorm* worm = (*iter)->getWorm();
		int x = (int)worm->getPos().x;
		int y = (int)worm->getPos().y;
		Culler<TestCuller, 1> testCuller(TestCuller(m_dest, -(int)m_pos.x, -(int)m_pos.y));
		//testCuller.cull();

		testCuller.cullRows(
			0,
			0,
			Culler<TestCuller, 1>::fix(0),
			Culler<TestCuller, 1>::fix(200),
			0,
			200,
			0,
			x,
			y
			);
		//flag = true;
	}
*/
#ifdef USE_GRID
	for ( Grid::iterator iter = game.objects.beginAll(); iter; ++iter)
	{
		iter->draw(m_dest, offX, offY);
	}
#else
	for ( int i = 0; i < RENDER_LAYERS_AMMOUNT ; ++i)
	{
		ObjectsList::RenderLayerIterator iter;
		for ( iter = game.objects.renderLayerBegin(i); (bool)iter; ++iter)
		{
			(*iter)->draw(m_dest, offX, offY);
		}
	}
#endif

	EACH_CALLBACK(i, wormRender)
	{
		for(list<BasePlayer*>::iterator playerIter = game.players.begin(); playerIter != game.players.end(); ++playerIter)
		{
			BaseWorm* worm = (*playerIter)->getWorm();
			if( worm && worm->isActive() )
			{
				Vec renderPos = worm->getRenderPos();
				int x = (int)renderPos.x - offX;
				int y = (int)renderPos.y - offY;
				bool ownViewport = (*playerIter == player);
				lua.callReference(*i, (lua_Number)x, (lua_Number)y, worm->luaReference, luaReference, ownViewport);
			}
		}
		
	}
	
	if(BaseWorm* worm = player->getWorm())
	{
		EACH_CALLBACK(i, viewportRender)
		{
			lua.callReference(*i, luaReference, worm->luaReference);
		}
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
