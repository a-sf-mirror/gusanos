#ifndef WORM_H
#define WORM_H


#include "base_object.h"
#include "sprite.h"

class BaseAnimator;
class BasePlayer;

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
	
	void assignOwner( BasePlayer* owner);

	void draw(BITMAP* where,int xOff, int yOff);
	void think();
	void actionStart( Actions action );
	void actionStop( Actions action );
	void addAimSpeed(float speed);
	
	private:
	
	bool movingLeft;
	bool movingRight;
	bool jumping;
	
	int dir;
	float aimAngle;
	float aimSpeed;
	float aimRecoilSpeed;
	
	BasePlayer* m_owner;
	
	Sprite *skin;
	BaseAnimator* m_animator;
};

#endif  // _WORM_H_
