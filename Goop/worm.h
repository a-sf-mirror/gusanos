#ifndef WORM_H
#define WORM_H

#include "vec.h"
#include "base_object.h"
#include "sprite.h"

class BaseAnimator;
class BasePlayer;
class NinjaRope;

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
		DIG,
		NINJAROPE
	};
		
	Worm();
	~Worm();
	
	void assignOwner( BasePlayer* owner);

	void draw(BITMAP* where,int xOff, int yOff);
	void think();
	void actionStart( Actions action );
	void actionStop( Actions action );
	void addAimSpeed(float speed);
	
	Vec getPos();
	float getAngle();
	char getDir();
	
	BaseObject* getNinjaRopeObj();
	
	private:
	
	bool movingLeft;
	bool movingRight;
	bool jumping;
	
	int dir;
	float aimAngle;
	float aimSpeed;
	float aimRecoilSpeed;
	
	int currentWeapon;
	
	std::vector<Weapon*> m_weapons;
	
	BasePlayer* m_owner;
	NinjaRope* m_ninjaRope;
	
	Sprite *skin;
	BaseAnimator* m_animator;
};

#endif  // _WORM_H_
