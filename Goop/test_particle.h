#ifndef test_particle_h
#define test_particle_h


#include "base_object.h"
#include "sprite.h"

class TestParticle : public BaseObject
{
	public:
		
	TestParticle();

	void draw(BITMAP* where,int xOff, int yOff);
	void think();
	
	private:
	
	Sprite *sprite;
};

#endif  // _base_object_h_
