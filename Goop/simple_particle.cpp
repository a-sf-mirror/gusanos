#include "simple_particle.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#include "gfx.h"

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
	if(!game.level.getMaterial(int(pos.x), int(pos.y)).particle_pass
	|| --timeout == 0)
		deleteMe = true;
	
	pos += spd;
	
	/*
	spdy += gravity;
	
	if(!game.level.getMaterial(posx >> 8, posy >> 8).particle_pass
	|| --timeout == 0)
		deleteMe = true;
		
	posx += spdx;
	posy += spdy;
	*/
}

void SimpleParticle::draw(BITMAP* where, int xOff, int yOff)
{
	putpixel(where, (int)(pos.x)-xOff, (int)(pos.y)-yOff, colour);
	//putpixel(where, (posx >> 8)-xOff, (posy >> 8)-yOff, colour);
}

void SimpleParticle32::draw(BITMAP* where, int xOff, int yOff)
{
	int x = (int)(pos.x)-xOff;
	int y = (int)(pos.y)-yOff;
	/*
	int x = (posx >> 8)-xOff;
	int y = (posy >> 8)-yOff;*/
	if((unsigned int)x < where->w
	&& (unsigned int)y < where->h)
		((unsigned long *)where->line[y])[x] = colour;
}