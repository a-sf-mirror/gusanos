#ifndef WORM_H
#define WORM_H


#include "base_object.h"
#include "sprite.h"

class BaseAnimator;

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
	void addToAim(float angle);
	void jumpStart();
	void jumpStop();
	
	private:
	
	bool movingLeft;
	bool movingRight;
	bool jumping;
	
	int dir;
	float aimAngle;
	float frame;
	Sprite *skin;
	BaseAnimator* m_animator;
};

#endif  // _WORM_H_
