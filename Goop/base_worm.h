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
		RESPAWN
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
	
	virtual void assignOwner( BasePlayer* owner);

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

	float getHealth();
	
	void damage( float amount );
	virtual void respawn();
	void respawn(const Vec& newPos);
	virtual void die();
	
	float getAngle();
	char getDir();
	void setDir(char d);
	bool isCollidingWith( const Vec& point, float radius );
	bool isActive();
	
	NinjaRope* getNinjaRopeObj();
	
	float aimSpeed; // Useless to add setters and getters for this
	
protected:

	Vec renderPos;
	
	bool m_isActive;
	
	bool movingLeft;
	bool movingRight;
	bool jumping;
	
	int dir;
	int reacts[DirMax];
	bool animate;
	bool movable; // What do we need this for? // Dunno, did I put this here? :o
	bool changing;
	float aimAngle;
	float aimRecoilSpeed;
	float health;
	//float currentRopeLength; //moved to Ninjarope
	
	size_t currentWeapon;
	
	std::vector<Weapon*> m_weapons;
	
	BasePlayer* m_owner;
	NinjaRope* m_ninjaRope;
	
	Sprite *skin;
	BaseAnimator* m_animator;
};

#endif  // _WORM_H_
