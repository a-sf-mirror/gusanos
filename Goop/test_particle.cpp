#include "test_particle.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"
#include "sprite.h"

TestParticle::TestParticle()
{
	do
	{
		pos.x = rand()%game.level.width();
		pos.y = rand()%game.level.height();
	} while ( !game.level.getMaterial((int) pos.x,(int) pos.y).particle_pass );
	
	spd.x = (rand()%100)/100. - 0.5;
	spd.y = (rand()%100)/100. - 0.5;
	
	sprite = spriteList.load("skin.bmp");
}

void TestParticle::think()
{
	spd.y+=0.01;
	if ( !game.level.getMaterial( (int)(pos+spd).x, (int)pos.y ).particle_pass)
		spd.x*=-0.9;
	if ( !game.level.getMaterial( (int)pos.x, (int)(pos+spd).y ).particle_pass)
		spd.y*=-0.9;
	pos = pos + spd;
}

void TestParticle::draw(BITMAP* where,int xOff, int yOff)
{
	putpixel(where,(int)(pos.x)-xOff,(int)(pos.y)-yOff,makecol(255,255,255));
	//sprite->drawCenter(where, rand()%25, (int)pos.x-xOff, (int)pos.y-yOff);
}
