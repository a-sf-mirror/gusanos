#ifndef test_particle_h
#define test_particle_h


#include "base_object.h"

class TestParticle : public BaseObject
{
	public:
		
	TestParticle();

	void draw(BITMAP* where,int xOff, int yOff);
	void think();
};

#endif  // _base_object_h_
