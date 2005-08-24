#include "level.h"

#include "gfx.h"
#include "material.h"
#include "liero.h"
#include "sprite.h"

#include <allegro.h>
#include <string>
#include <vector>

using namespace std;

ResourceLocator<Level> levelLocator;

Level::Level()
{
	loaded = false;
	
	image = NULL;
	material = NULL;
	background = NULL;
	
	m_materialList.insert( m_materialList.begin(), 257, Material() );

	m_materialList[0].worm_pass = false;
	m_materialList[0].particle_pass = false;
	
	m_materialList[1].worm_pass = false;
	m_materialList[1].particle_pass = false;
	m_materialList[1].draw_exps = false;
}

Level::~Level()
{
}
/*
bool Level::load(const string &name)
{
	//check if it is liero level
	if (name.size() > 4)
		if (name.substr(name.size() - 4, name.size()) == ".lev")
			if (loadLiero(name))
				return true;

	//load Gusanos level
	path = name;
	int vdepth=get_color_depth();
	set_color_depth(8);
	material = gfx.loadBitmap((name + "/material").c_str(),0);
	set_color_depth(vdepth);
	if (material)
	{
		image = gfx.loadBitmap((name + "/level").c_str(),0);
		if (image)
		{
			background = gfx.loadBitmap((name + "/background").c_str(),0);
			if(!background)
			{
				background = create_bitmap(material->w, material->h);
				for (int x = 0; x < background->w; x++)
				for (int y = 0; y < background->h; y++)
				{
					putpixel(background, x, y, getpixel(image, x, y));
				}
			}
			loaded = true;
			return true;
		}
	}
	unload();
	return false;
}

bool Level::loadLiero(const std::string &name)
{
	path = name;

	Liero::LieroLevel lev;
	Liero::LieroColor pal[256];
	if (!Liero::loadPalette(pal, "original.lpl"))
		return false;
	if (!Liero::loadLevel(&lev, pal, name))
		return false;

	material = create_bitmap_ex(8, Liero::MAP_WIDTH, Liero::MAP_HEIGHT);
	image = create_bitmap_ex(16, Liero::MAP_WIDTH, Liero::MAP_HEIGHT);
	background = create_bitmap_ex(16, Liero::MAP_WIDTH, Liero::MAP_HEIGHT);

	for (int y = 0; y < Liero::MAP_HEIGHT; y++)
	{
		for (int x = 0; x < Liero::MAP_WIDTH; x++)
		{
			int c = lev.level[y][x];
			int crgb = makecol(pal[c].r, pal[c].g, pal[c].b);
			int m = 1;

			putpixel(image, x, y, crgb);
			putpixel(background, x, y, crgb);

			//material
			//dirt
			if (c >= 12 && c <= 18 || c == 1 || c == 2)
				m = 0;
			else
			//green dirt
			if (c >= 94 && c <= 97)
				m = 0;
			else
			//rock
			if (c >= 98 && c <= 103)
				m = 0;
			else
			//diggable stone
			if (c >= 176 && c <= 180)
				m = 0;
			else
			//rock
			if (c >= 19 && c <= 29 || c == 8)
				m = 0;
			else
			//nothing
				m = 1;
			putpixel(material, x, y, m);
		}
	}
	loaded = true;
	return true;
}
*/
void Level::unload()
{
	loaded = false;
	path = "";
	if (image) destroy_bitmap(image);
	if (material) destroy_bitmap(material);
	if (background) destroy_bitmap(background);
	image = NULL;
	material = NULL;
	background = NULL;
	vectorEncoding = Encoding::VectorEncoding();
}

bool Level::isLoaded()
{
	return loaded;
}

void Level::draw(BITMAP* where, int x, int y)
{
	if (image)
	{
		blit(image,where,x,y,0,0,where->w,where->h);
	}
}

// TODO: optimize this
void Level::specialDrawSprite( Sprite* sprite, BITMAP* where, const Vec& pos, const Vec& matPos )
{
	int transCol = makecol(255,0,255); // TODO: make a gfx.getTransCol() function

	int xMatStart = (int)matPos.x - sprite->m_xPivot;
	int yMatStart = (int)matPos.y - sprite->m_yPivot;
	int xDrawStart = (int)pos.x - sprite->m_xPivot;
	int yDrawStart = (int)pos.y - sprite->m_yPivot;
	for ( int x = 0; x < sprite->m_bitmap->w ; ++x )
	for ( int y = 0; y < sprite->m_bitmap->h ; ++y )
	{
		if ( getMaterial ( xMatStart + x , yMatStart + y ).draw_exps )
		{
			//int c = sprite->m_bitmap->line[y][x];
			int c = getpixel( sprite->m_bitmap, x, y );
			if ( c != transCol ) putpixel( where, xDrawStart + x, yDrawStart + y, c );
		}
	}
}

Vec Level::getSpawnLocation()
{
	Vec pos = Vec(rnd() * material->w, rnd()*material->h);
	while ( !getMaterial( pos.x, pos.y ).worm_pass )
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
	// Make the domain one pixel larger than the level so that things like ninjarope hook
	// can get slightly outside the level and attach.
	vectorEncoding = Encoding::VectorEncoding(Rect(-1, -1, width() + 1, height() + 1), 2048);
	cerr << "vectorEncoding: " << vectorEncoding.totalBits() << endl;
}