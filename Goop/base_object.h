#ifndef base_object_h
#define base_object_h

#include "vec.h"
#include "angle.h"

//#include <allegro.h>
struct BITMAP;

class BasePlayer;

class BaseObject 
{
public:
		
	BaseObject( BasePlayer* owner = NULL, Vec pos_ = Vec(), Vec spd_ = Vec() );
	virtual ~BaseObject();

	// Draw the object in the where bitmap with an offset ( used for camera )
	virtual void draw(BITMAP* where, int xOff, int yOff) {}
	
	// All the object logic here
	virtual void think() {}
	
	// Gets the position on the level where the object wants to be rendered
	virtual Vec getRenderPos();
	
	// Gets a pointer to the Player that owns this object ( NULL if it has no owner )
	virtual BasePlayer* getOwner();
	
	// Gets the angle the object is pointing
	virtual Angle getAngle();
	
	// Gets the object's dir ( left = -1 right = 1 )
	virtual int getDir();
	
	// Returns true if the object is colliding with a circle of the given radius in the given point
	virtual bool isCollidingWith( const Vec& point, float radius );
	
	// Removes all the connections this object may have to the given player
	virtual void removeRefsToPlayer( BasePlayer* player );
	
	// Adds the speed value to the current angle speed of the object
	virtual void addAngleSpeed( AngleDiff speed ) {}
	
	// Tells the object to remove itself ( The object may not agree and nothing will happen )
	virtual void remove();
	
	// Sets a destination alpha value and the time in logic frames it will take to reach that value
	virtual void setAlphaFade( int frames, int dest ) {}
	
	// Runs the custom event number "index" of the object
	virtual void customEvent ( size_t index ) {}
	
	// Effects amount damage to the object and sets the last damager to the passed BasePlayer pointer
	virtual void damage(float amount, BasePlayer* damager ) {}
	
	// Adds the speed vector to the current speed
	virtual void addSpeed( Vec spd_ )
	{ spd += spd_; }
	
	// Dunno what this does.. gliptiiiic!! ;>
	virtual void pushLuaReference();
	
	// If this is true the object will be removed from the objects list in the next frame
	bool deleteMe;
	
	// IMPORTANT: The pos and spd vectors should be used as read only. ( Because of netplay needs )
	// To change their values use the setters provided.
	Vec pos; 
	Vec spd;
	
	int luaData;

	BaseObject* nextS_;
	BaseObject* nextD_;
	BaseObject* prevD_;
	int cellIndex_;
	
protected:
	
	BasePlayer* m_owner;
};


#endif  // _base_object_h_
