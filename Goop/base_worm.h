#ifndef BASE_WORM_H
#define BASE_WORM_H

#include "vec.h"
#include "base_object.h"

class BaseAnimator;
class BasePlayer;
class NinjaRope;
class Weapon;
class SpriteSet;

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
	
	void damage( float amount, BasePlayer* damager );
	
	// This are virtual so that NetWorm can know about them and tell others over the network.
	virtual void respawn();
	void respawn(const Vec& newPos);
	virtual void die();
	virtual void changeWeaponTo( unsigned int weapIndex );
	
	Weapon* getCurrentWeapon(); // Where and what for is this used?
	
	// getWeaponIndexOffset can be used to get the currentWeapon index or 
	//to get the one to the right or the left or the one 1000 units to the 
	//right ( it will wrap the value so that its always inside the worm's weapons size )
	int getWeaponIndexOffset( int offset );
	float getAngle();
	void setDir(char d);
	bool isCollidingWith( const Vec& point, float radius );
	bool isActive();
	void removeRefsToPlayer( BasePlayer* player );
	
	void showFirecone( SpriteSet* sprite, int frames, float distance );
	
	NinjaRope* getNinjaRopeObj();
	
	float aimSpeed; // Useless to add setters and getters for this
	float aimAngle;
	
protected:

	Vec renderPos;
	
	bool m_isActive;
	
	bool movingLeft;
	bool movingRight;
	bool jumping;

	int reacts[DirMax];
	bool animate;
	bool movable; // What do we need this for? // Dunno, did I put this here? :o
	bool changing; // This shouldnt be in the worm class ( its player stuff >:O )
	float aimRecoilSpeed;
	float health;
	//float currentRopeLength; //moved to Ninjarope
	
	int m_fireconeTime;
	float m_fireconeDistance;
	
	int m_timeSinceDeath; // Used for the min and max respawn time sv variables
	
	size_t currentWeapon;
	
	std::vector<Weapon*> m_weapons;
	
	BasePlayer* m_lastHurt;
	NinjaRope* m_ninjaRope;
	
	SpriteSet *skin;
	SpriteSet *m_currentFirecone;
	BaseAnimator* m_fireconeAnimator;
	BaseAnimator* m_animator;
};

#endif  // _WORM_H_
