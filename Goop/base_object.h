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
	
	virtual Vec getPos();
	virtual Vec getSpd();
	virtual float getAngle();
	virtual void addAngleSpeed( float speed ) {};
	
	bool deleteMe;
	
	protected:
		
	Vec pos;
	Vec spd;
};

#endif  // _base_object_h_
