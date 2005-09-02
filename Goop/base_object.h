#ifndef base_object_h
#define base_object_h

#include "vec.h"

//#include <allegro.h>
struct BITMAP;

class BasePlayer;

class BaseObject 
{
	public:
		
	BaseObject( BasePlayer* owner = NULL, int dir = 1 );
	virtual ~BaseObject();

	virtual void draw(BITMAP* where,int xOff, int yOff) = 0;
	virtual void think() = 0;
	
	virtual Vec getPos();
	virtual Vec getRenderPos();
	virtual Vec getSpd();
	virtual BasePlayer* getOwner();
	virtual float getAngle();
	virtual int getDir();
	virtual void addAngleSpeed( float speed ) {}
	virtual void remove();
	virtual bool isCollidingWith( const Vec& point, float radius );
	virtual void removeRefsToPlayer( BasePlayer* player );
	virtual void setAlphaFade( int frames, int dest ) {};
	virtual void customEvent ( size_t index ) {};
	virtual void damage(float amount, BasePlayer* damager ) {};
	
	void addSpeed( Vec spd_ )
	{ spd += spd_; }
	
	bool deleteMe;
	
	Vec pos;
	Vec spd;
	
	protected:
	
	int m_dir;
	BasePlayer* m_owner;
};


#endif  // _base_object_h_
