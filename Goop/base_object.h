#ifndef base_object_h
#define base_object_h

#include "vec.h"

#include <allegro.h>

class BaseObject
{
	public:
		
	BaseObject();

	virtual void draw(BITMAP* where,int xOff, int yOff) = 0;
	virtual void think() = 0;
	
	protected:
		
	Vec pos;
	Vec spd;
};

#endif  // _base_object_h_
