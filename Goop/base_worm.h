#ifndef BASE_WORM_H
#define BASE_WORM_H

#include "vec.h"
#include "base_object.h"
#include "sprite.h"

class BaseAnimator;
class BasePlayer;
class NinjaRope;

class BaseWorm : public BaseObject
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
		DIG,
		NINJAROPE
	};
		
	BaseWorm();
	virtual ~BaseWorm();
	
	void assignOwner( BasePlayer* owner);

	void draw(BITMAP* where,int xOff, int yOff);
	virtual void think();
	void actionStart( Actions action );
	void actionStop( Actions action );
	void addAimSpeed(float speed);
	void addRopeLength(float distance);
	
	Vec getPos();
	Vec getRenderPos();
	float getAngle();
	char getDir();
	
	NinjaRope* getNinjaRopeObj();
	
	protected:

	Vec renderPos;
	
	bool movingLeft;
	bool movingRight;
	bool jumping;
	
	int dir;
	float aimAngle;
	float aimSpeed;
	float aimRecoilSpeed;
	float currentRopeLength;
	
	int currentWeapon;
	
	std::vector<Weapon*> m_weapons;
	
	BasePlayer* m_owner;
	NinjaRope* m_ninjaRope;
	
	Sprite *skin;
	BaseAnimator* m_animator;
};

#endif  // _WORM_H_
