#ifndef WORM_H
#define WORM_H


#include "base_object.h"
#include "sprite.h"

class BaseAnimator;

class Worm : public BaseObject
{
	public:
		
	enum Actions
	{
		MOVELEFT,
		MOVERIGHT,
		AIMUP,
		AIMDOWN,
		FIRE,
		FIRE2,
		JUMP,
		DIG
	};
		
	Worm();
	~Worm();

	void draw(BITMAP* where,int xOff, int yOff);
	void think();
	void actionStart( Actions action );
	void actionStop( Actions action );
	void addToAim(float angle);
	
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
