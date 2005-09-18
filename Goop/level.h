#ifndef LEVEL_H
#define LEVEL_H

#include "material.h"
#include "resource_locator.h"
#include "vec.h"
//#include "sprite.h"

#include "encoding.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <cmath>
#include <boost/array.hpp>
using boost::array;

class Sprite;
class LevelEffect;
struct BlitterContext;

class Level
{
	public:
		
	Level();
	~Level();
	/*
	bool load(const std::string &name);
	bool loadLiero(const std::string &name);*/
	void unload();
	bool isLoaded();
	void draw(BITMAP* where, int x, int y);
	int width();
	int height();
	
	const std::string &getPath();
	const std::string &getName();
	void setName(const std::string &_name);
	/*
	const Material& getMaterial(int x, int y);*/
	
	Vec getSpawnLocation();
	
	Material const& getMaterial(unsigned int x, unsigned int y)
	{
		if(x < static_cast<unsigned int>(material->w) && y < static_cast<unsigned int>(material->h))
			return m_materialList[(unsigned char)material->line[y][x]];
		else
			return m_materialList[0];
	}
	
	Material const& unsafeGetMaterial(unsigned int x, unsigned int y)
	{
		return m_materialList[(unsigned char)material->line[y][x]];
	}
	
	bool isInside(unsigned int x, unsigned int y)
	{
		if(x < static_cast<unsigned int>(material->w) && y < static_cast<unsigned int>(material->h))
			return true;
		else
			return false;
	}
	
	template<class PredT>
	bool trace(long srcx, long srcy, long destx, long desty, PredT predicate);
	
	void specialDrawSprite(Sprite* sprite, BITMAP* where, const Vec& pos, const Vec& matPos, BlitterContext const& blitter );
	
	// applies the effect and returns true if it actually changed something on the map
	bool applyEffect( LevelEffect* effect, int x, int y);
	
	void loaderSucceeded();
	
	//private:
		
	bool loaded;
	
	BITMAP* image;
	BITMAP* material;
	BITMAP* background;
	std::string name;
	std::string path;
	array<Material, 256> m_materialList;
	Encoding::VectorEncoding vectorEncoding;
	
	struct ParticleBlockPredicate
	{
		bool operator()(Material const& m)
		{
			return !m.particle_pass;
		}
	};
};

#define SIGN(x_) ((x_) < 0 ? -1 : (x_) > 0 ? 1 : 0)

template<class PredT>
bool Level::trace(long x, long y, long destx, long desty, PredT predicate)
{
	if(!isInside(x, y))
	{
		if(predicate(m_materialList[0]))
			return true;
		else
		{
			return true; //TODO: Clip the beginning of the line instead of returning
		}
	}
	if(!isInside(destx, desty))
	{
		if(predicate(m_materialList[0]))
			return true;
		else
		{
			return true; //TODO: Clip the end of the line instead of returning
		}
	}
		
	long xdiff = destx - x;
	long ydiff = desty - y;
	
	long sx = SIGN(xdiff);
	long sy = SIGN(ydiff);

	xdiff = labs(xdiff);
	ydiff = labs(ydiff);
	
	#define WORK(a, b) { \
		long i = a##diff >> 1; \
		long c = a##diff; \
		while(c-- >= 0) { \
			if(predicate(unsafeGetMaterial(x, y))) return true; \
			i -= b##diff; \
			a += s##a; \
			if(i < 0) b += s##b, i += a##diff; } }
	
	if(xdiff > ydiff)
		WORK(x, y)
	else
		WORK(y, x)

	#undef WORK

	return false;
}

#undef SIGN

extern ResourceLocator<Level> levelLocator;

#endif // _LEVEL_H_
