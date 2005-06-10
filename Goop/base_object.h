#ifndef base_object_h
#define base_object_h

#include "vec.h"

#include <allegro.h>

class BasePlayer;

class BaseObject
{
	public:
		
	BaseObject( BasePlayer* owner = NULL );
	virtual ~BaseObject();

	virtual void draw(BITMAP* where,int xOff, int yOff) = 0;
	virtual void think() = 0;
	
	virtual Vec getPos();
	virtual Vec getRenderPos();
	virtual Vec getSpd();
	virtual BasePlayer* getOwner();
	virtual float getAngle();
	virtual void addAngleSpeed( float speed ) {}
	virtual void remove();
	virtual bool isCollidingWith( const Vec& point, float radius );
	virtual void removeRefsToPlayer( BasePlayer* player );
	
	void addSpeed( Vec spd_ )
	{ spd += spd_; }
	
	bool deleteMe;
	
	Vec pos;
	Vec spd;
	
	protected:
	
	BasePlayer* m_owner;
};

#endif  // _base_object_h_
