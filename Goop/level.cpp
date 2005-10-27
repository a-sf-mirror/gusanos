#include "level.h"

#ifndef DEDSERV
#include "gfx.h"
#include "blitters/context.h"
#endif
#include "material.h"
#include "sprite_set.h"
#include "sprite.h"
#include "omfgutil_math.h"
#include "omfgutil_macros.h"
#include "level_effect.h"
#include "viewport.h"
#include "culling.h"


#include <allegro.h>
#include <string>
#include <vector>

using namespace std;

ResourceLocator<Level> levelLocator;


struct AddCuller
{
	AddCuller( Level& level, BITMAP* dest, BITMAP* source, int alpha,int dOffx, int dOffy, int sOffx, int sOffy ) :
		m_level(level),
		m_dest( dest ),
		m_source( source),
		m_alpha(alpha),
		m_destOffx(dOffx),
		m_destOffy(dOffy),
		m_sourceOffx(sOffx),
		m_sourceOffy(sOffy)
	{
	};
	
	bool block(int x, int y)
	{
		return m_level.unsafeGetMaterial(x,y).blocks_light;
	}
		
	void line(int y, int x1, int x2)
	{
		drawSpriteLine_add(
			m_dest,
			m_source,
			x1 - m_destOffx,
			y - m_destOffy,
			x1 - m_sourceOffx,
			y - m_sourceOffy,
			x2 - m_sourceOffx + 1,
			m_alpha
		);
	}

private:
	
	Level const &m_level;
	
	BITMAP* m_dest;
	BITMAP* m_source;
	
	int m_destOffx;
	int m_destOffy;
	int m_sourceOffx;
	int m_sourceOffy;

	int m_alpha;
	
};

Level::Level()
{
	loaded = false;
	
#ifndef DEDSERV
	image = NULL;
	background = NULL;	
	paralax = NULL;
	lightmap = NULL;
#endif
	material = NULL;
	
	for ( int i = 0; i < m_materialList.size() ; ++i )
	{
		m_materialList[i].index = i;
	}

	// Rock
	m_materialList[0].worm_pass = false;
	m_materialList[0].particle_pass = false;
	m_materialList[0].draw_exps = false;
	m_materialList[0].blocks_light = true;
	
	// Background
	m_materialList[1].worm_pass = true;
	m_materialList[1].particle_pass = true;
	m_materialList[1].draw_exps = true;
	
	// Dirt
	m_materialList[2].worm_pass = false;
	m_materialList[2].particle_pass = false;
	m_materialList[2].draw_exps = false;
	m_materialList[2].destroyable = true;
	m_materialList[2].blocks_light = true;
	m_materialList[2].flows = false;
	
	// Special dirt
	m_materialList[3].worm_pass = true;
	m_materialList[3].particle_pass = false;
	m_materialList[3].draw_exps = false;
	m_materialList[3].destroyable = true;
	
	// Special rock
	m_materialList[4].worm_pass = false;
	m_materialList[4].particle_pass = true;
	m_materialList[4].draw_exps = false;
	
	m_materialList[7].worm_pass = false;
	m_materialList[7].particle_pass = false;
	m_materialList[7].draw_exps = false;
}

Level::~Level()
{
}

void Level::unload()
{
	loaded = false;
	path = "";

#ifndef DEDSERV
	destroy_bitmap(image); image = NULL;
	destroy_bitmap(background); background = NULL;
	destroy_bitmap(paralax); paralax = NULL;
	destroy_bitmap(lightmap); lightmap = NULL;
#endif
	destroy_bitmap(material); material = NULL;

	vectorEncoding = Encoding::VectorEncoding();
}

bool Level::isLoaded()
{
	return loaded;
}

void Level::think()
{
	foreach_delete( wp, m_water )
	{
		if ( getMaterialIndex( wp->x, wp->y ) != wp->mat )
		{
				putpixel_solid(image, wp->x, wp->y, getpixel(background, wp->x, wp->y) );
				m_water.erase(wp);
		}else
		if ( rnd() > WaterSkipFactor )
		{
			if ( getMaterial( wp->x, wp->y+1 ).particle_pass )
			{
				putpixel_solid(image, wp->x, wp->y, getpixel(background, wp->x, wp->y) );
				putMaterial( 1, wp->x, wp->y );
				++wp->y;
				putpixel_solid(image, wp->x, wp->y, makecol(0,0,100) );
				putMaterial( wp->mat, wp->x, wp->y );
			}else
			{
				char dir;
				if ( wp->dir ) dir = 1;
				else dir = -1;
				
				if ( getMaterial( wp->x+dir, wp->y ).particle_pass )
				{
					putpixel_solid(image, wp->x, wp->y, getpixel(background, wp->x, wp->y) );
					putMaterial( 1, wp->x, wp->y );
					wp->x += dir;
					putpixel_solid(image, wp->x, wp->y, makecol(0,0,100) );
					putMaterial( wp->mat, wp->x, wp->y );
				}
				else
					wp->dir = !wp->dir;
			}
		}
	}
}

#ifndef DEDSERV
void Level::draw(BITMAP* where, int x, int y)
{
	if (image)
	{
		if (!paralax)
		{
			blit(image,where,x,y,0,0,where->w,where->h);
		}else
		{
			int px = x * (paralax->w - where->w) / (float)( material->w - where->w );
			int py = y * (paralax->h - where->h) / (float)( material->h - where->h );
			blit(paralax,where,px,py,0,0,where->w,where->h);
			masked_blit(image,where,x,y,0,0,where->w,where->h);
		}
	}
}


// TODO: optimize this
void Level::specialDrawSprite( Sprite* sprite, BITMAP* where, const IVec& pos, const IVec& matPos, BlitterContext const& blitter )
{
	int transCol = makecol(255,0,255); // TODO: make a gfx.getTransCol() function

	int xMatStart = matPos.x - sprite->m_xPivot;
	int yMatStart = matPos.y - sprite->m_yPivot;
	int xDrawStart = pos.x - sprite->m_xPivot;
	int yDrawStart = pos.y - sprite->m_yPivot;
	for ( int x = 0; x < sprite->m_bitmap->w ; ++x )
	for ( int y = 0; y < sprite->m_bitmap->h ; ++y )
	{
		if ( getMaterial ( xMatStart + x , yMatStart + y ).draw_exps )
		{
			//int c = sprite->m_bitmap->line[y][x];
			int c = getpixel( sprite->m_bitmap, x, y );
			if ( c != transCol ) blitter.putpixel( where, xDrawStart + x, yDrawStart + y, c );
		}
	}
}

void Level::culledDrawSprite( Sprite* sprite, Viewport* viewport, const IVec& pos, int alpha )
{
	BITMAP* renderBitmap = sprite->m_bitmap;
	IVec off = viewport->getPos();
	IVec loff(pos - IVec(sprite->m_xPivot, sprite->m_yPivot));
	
	Rect r(0, 0, width() - 1, height() - 1);
	r &= Rect(renderBitmap) + loff;
	
	Culler<AddCuller> addCuller(
		AddCuller(
			*this,
			viewport->dest,
			renderBitmap,
			alpha,
			off.x,
			off.y,
			loff.x,
			loff.y
			),
		r );

	addCuller.cullOmni(pos.x, pos.y);
}

void Level::culledDrawLight( Sprite* sprite, Viewport* viewport, const IVec& pos, int alpha )
{
	BITMAP* renderBitmap = sprite->m_bitmap;
	IVec off = viewport->getPos();
	IVec loff(pos - IVec(sprite->m_xPivot, sprite->m_yPivot));
	
	Rect r(0, 0, width() - 1, height() - 1);
	r &= Rect(renderBitmap) + loff;
	
	Culler<AddCuller> addCuller(
		AddCuller(
			*this,
			viewport->testFade,
			renderBitmap,
			alpha,
			off.x,
			off.y,
			loff.x,
			loff.y
			),
		r );

	addCuller.cullOmni(pos.x, pos.y);
}

#endif

bool Level::applyEffect(LevelEffect* effect, int drawX, int drawY )
{
	bool returnValue = false;
	if ( effect && effect->mask )
	{
		Sprite* tmpMask = effect->mask->getSprite();
		drawX -= tmpMask->m_xPivot;
		drawY -= tmpMask->m_yPivot;
		unsigned int colour = 0;
		for( int x = 0; x < tmpMask->m_bitmap->w; ++x )
		for( int y = 0; y < tmpMask->m_bitmap->h; ++y )
		{
			colour = getpixel( tmpMask->m_bitmap, x, y);
			if( ( colour == 0 ) && getMaterial( drawX+x, drawY+y ).destroyable )
			{
				returnValue = true;
				putMaterial( 1, drawX+x, drawY+y );
#ifndef DEDSERV
				putpixel(image, drawX+x, drawY+y, getpixel( background, drawX+x, drawY+y ) );
#endif
			}
		}
	}
	return returnValue;
}

Vec Level::getSpawnLocation()
{
	Vec pos = Vec(rnd() * material->w, rnd()*material->h);
	while ( !getMaterial( static_cast<int>(pos.x), static_cast<int>(pos.y) ).worm_pass )
	{
		pos = Vec(rnd() * material->w, rnd()*material->h);
	}
	return pos;
}

/*
const Material& Level::getMaterial(int x, int y)
{
	return m_materialList[getpixel(material,x,y)+1];
}
*/

int Level::width()
{
	if ( material )
		return material->w;
	else
		return 0;
}

int Level::height()
{
	if ( material )
		return material->h;
	else
		return 0;
}

void Level::setName(const std::string& _name)
{
	name = _name;
}

const string& Level::getPath()
{
	return path;
}

const string& Level::getName()
{
	return name;
}

void Level::loaderSucceeded()
{
	loaded = true;
	
	for ( int x = 0; x < material->w; ++x )
	for ( int y = 0; y < material->h; ++y )
	{
		if ( unsafeGetMaterial(x,y).flows )
		{
			m_water.push_back( WaterParticle( x, y, getMaterialIndex(x,y) ) );
		}
	}
	
	if ( !lightmap )
	{
		LocalSetColorDepth cd(8);
		lightmap = create_bitmap(material->w, material->h);
		clear_to_color(lightmap, 200);
		/*for ( int x = 0; x < lightmap->w ; ++x )
		for ( int y = 0; y < lightmap->h ; ++y )
		{
			if ( unsafeGetMaterial(x,y).blocks_light )
				putpixel( lightmap, x, y, 200 );
		}*/
	}
	
	// Make the domain one pixel larger than the level so that things like ninjarope hook
	// can get slightly outside the level and attach.
	vectorEncoding = Encoding::VectorEncoding(Rect(-1, -1, width() + 1, height() + 1), 2048);
	intVectorEncoding = Encoding::VectorEncoding(Rect(-1, -1, width() + 1, height() + 1), 1);
	diffVectorEncoding = Encoding::DiffVectorEncoding(1024);
	cerr << "vectorEncoding: " << vectorEncoding.totalBits() << endl;
}
