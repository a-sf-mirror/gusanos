#include "test_particle.h"

#include "vec.h"
#include "game.h"
#include "base_object.h"

TestParticle::TestParticle()
{
	do
	{
		pos.x = rand()%game.level.width();
		pos.y = rand()%game.level.height();
	} while ( !game.level.getMaterial((int) pos.x,(int) pos.y).particle_pass );
	
	spd.x = (rand()%100)/100. - 0.5;
	spd.y = (rand()%100)/100. - 0.5;
}

void TestParticle::think()
{
	if ( !game.level.getMaterial( (int)(pos+spd).x, (int)pos.y ).particle_pass)
		spd.x*=-1;
	if ( !game.level.getMaterial( (int)pos.x, (int)(pos+spd).y ).particle_pass)
		spd.y*=-1;
	pos = pos + spd;
}

void TestParticle::draw(BITMAP* where,int xOff, int yOff)
{
	putpixel(where,(int)(pos.x)-xOff,(int)(pos.y)-yOff,makecol(255,255,255));
}
