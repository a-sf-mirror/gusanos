#ifndef WORM_H
#define WORM_H


#include "base_object.h"
#include "sprite.h"

class Worm : public BaseObject
{
	public:
		
	Worm();
	~Worm();

	void draw(BITMAP* where,int xOff, int yOff);
	void think();
	void moveLeftStart();
	void moveLeftStop();
	void moveRightStart();
	void moveRightStop();
	void setAim(float angle);
	void jumpStart();
	void jumpStop();
	
	private:
	
	bool movingLeft;
	bool movingRight;
	bool jumping;
	int dir;
	Sprite *skin;
};

#endif  // _WORM_H_
