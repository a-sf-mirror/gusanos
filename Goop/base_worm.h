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
		NINJAROPE,
		CHANGEWEAPON,
		CHANGELEFT,
		CHANGERIGHT,
	};
	
	enum Direction
	{
		Down = 0,
		Left,
		Up,
		Right,
		
		DirMax
	};
		
	BaseWorm();
	virtual ~BaseWorm();
	
	void assignOwner( BasePlayer* owner);

	void draw(BITMAP* where,int xOff, int yOff);
	
	void calculateReactionForce(BaseVec<long> origin, Direction dir);
	void calculateAllReactionForces(BaseVec<float>& nextPos, BaseVec<long>& inextPos);
	void processMoveAndDig(void);
	void processPhysics();
	void processJumpingAndNinjaropeControls();
	
	virtual void think();
	void actionStart( Actions action );
	void actionStop( Actions action );
	void addAimSpeed(float speed);
	void addRopeLength(float distance);
	
	Vec getPos();
	Vec getWeaponPos();
	Vec getRenderPos();
	float getAngle();
	char getDir();
	
	NinjaRope* getNinjaRopeObj();
	
	float aimSpeed; // Useless to add setters and getters for this
	
protected:

	Vec renderPos;
	
	bool movingLeft;
	bool movingRight;
	bool jumping;
	
	int dir;
	int reacts[DirMax];
	bool animate;
	bool movable; // What do we need this for?
	bool changing;
	float aimAngle;
	float aimRecoilSpeed;
	//float currentRopeLength; //moved to Ninjarope
	
	size_t currentWeapon;
	
	std::vector<Weapon*> m_weapons;
	
	BasePlayer* m_owner;
	NinjaRope* m_ninjaRope;
	
	Sprite *skin;
	BaseAnimator* m_animator;
};

#endif  // _WORM_H_
