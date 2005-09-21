#include "simple_particle.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#ifndef DEDSERV
#include "gfx.h"
#include "blitters/blitters.h"
#endif

#define BOOST_NO_MT
#include <boost/pool/pool.hpp>

static boost::pool<> particlePool(sizeof(SimpleParticle));

void* SimpleParticle::operator new(size_t count)
{
	assert(count <= sizeof(SimpleParticle));
	return particlePool.malloc();
}


void SimpleParticle::operator delete(void* block)
{
	particlePool.free(block);
}

void SimpleParticle::think()
{
	spd.y += gravity;
	Vec nextPos = pos + spd;
	if(!game.level.getMaterial(int(nextPos.x), int(nextPos.y)).particle_pass
	|| --timeout == 0)
		deleteMe = true;
	else
		pos = nextPos;
	
	/*
	spdy += gravity;
	
	if(!game.level.getMaterial(posx >> 8, posy >> 8).particle_pass
	|| --timeout == 0)
		deleteMe = true;
		
	posx += spdx;
	posy += spdy;
	*/
}

#ifndef DEDSERV
void SimpleParticle::draw(BITMAP* where, int xOff, int yOff)
{
	putpixel(where, (int)(pos.x)-xOff, (int)(pos.y)-yOff, colour);
	//putpixel(where, (posx >> 8)-xOff, (posy >> 8)-yOff, colour);
}

void SimpleParticle32::draw(BITMAP* where, int xOff, int yOff)
{
	int x = (int)(pos.x)-xOff;
	int y = (int)(pos.y)-yOff;
	

	if((unsigned int)x < where->w
	&& (unsigned int)y < where->h)
		Blitters::putpixel_solid_32(where, x, y, colour);
}

void SimpleParticle16::draw(BITMAP* where, int xOff, int yOff)
{
	int x = (int)(pos.x)-xOff;
	int y = (int)(pos.y)-yOff;
	

	if((unsigned int)x < where->w
	&& (unsigned int)y < where->h)
		Blitters::putpixel_solid_16(where, x, y, colour);
}

void SimpleParticle32wu::draw(BITMAP* where, int xOff, int yOff)
{
	Blitters::putpixelwu_blend_32(where, pos.x - xOff, pos.y - yOff, colour, 256);
}

void SimpleParticle16wu::draw(BITMAP* where, int xOff, int yOff)
{
	Blitters::putpixelwu_blend_16(where, pos.x - xOff, pos.y - yOff, colour, 32);
}

#endif