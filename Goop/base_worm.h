#ifndef BASE_WORM_H
#define BASE_WORM_H

#include "zoidcom.h"
#include "vec.h"
#include "base_object.h"
#include "angle.h"
#include <vector>
#include "lua/types.h"

class BasePlayer;
class NinjaRope;
class Weapon;
#ifndef DEDSERV
class SpriteSet;
class BaseAnimator;
#endif

class BaseWorm : public BaseObject
{	
public:

	enum Actions
	{
		MOVELEFT,
		MOVERIGHT,
		//AIMUP,
		//AIMDOWN,
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
	void addAimSpeed(AngleDiff speed);
	void addRopeLength(float distance);
	
	Vec getWeaponPos();
#ifndef DEDSERV
	Vec getRenderPos();
#endif

	float getHealth();
	
	virtual void damage( float amount, BasePlayer* damager );
	
	// This are virtual so that NetWorm can know about them and tell others over the network.
	virtual void respawn();
	void respawn(const Vec& newPos);
	
	virtual void dig();
	void dig(const Vec& digPos, Angle angle);
	
	virtual void die();
	virtual void changeWeaponTo( unsigned int weapIndex );
	
	void clearWeapons();
	
	Weapon* getCurrentWeapon(); // Where and what for is this used?
	
	// getWeaponIndexOffset can be used to get the currentWeapon index or 
	//to get the one to the right or the left or the one 1000 units to the 
	//right ( it will wrap the value so that its always inside the worm's weapons size )
	int getWeaponIndexOffset( int offset );
	Angle getAngle();
	void setDir(int d); // Only use this if you are going to sync it over netplay with an event
	int getDir() { return m_dir; }
	bool isCollidingWith( const Vec& point, float radius );
	bool isActive();
	void removeRefsToPlayer( BasePlayer* player );
	
#ifndef DEDSERV
	void showFirecone( SpriteSet* sprite, int frames, float distance );
#endif
	
	NinjaRope* getNinjaRopeObj();
	
	AngleDiff aimSpeed; // Useless to add setters and getters for this
	Angle aimAngle;
	LuaReference luaReference;
	
	virtual void sendWeaponMessage( int index, ZCom_BitStream* data ) {}
	virtual void pushLuaReference();
	
	void* operator new(size_t count);
	
	void operator delete(void* block);
	
protected:

#ifndef DEDSERV
	Vec renderPos;
#endif

	int reacts[DirMax];

	float aimRecoilSpeed;
	float health;
	//float currentRopeLength; //moved to Ninjarope
	
#ifndef DEDSERV
	int m_fireconeTime;
	float m_fireconeDistance;
#endif
	
	int m_timeSinceDeath; // Used for the min and max respawn time sv variables
	
	size_t currentWeapon;
	
	std::vector<Weapon*> m_weapons;
	int m_weaponCount;
	
	BasePlayer* m_lastHurt;
	NinjaRope* m_ninjaRope;
	
#ifndef DEDSERV
	SpriteSet *skin;
	SpriteSet *skinMask;
	SpriteSet *m_currentFirecone;
	BaseAnimator* m_fireconeAnimator;
	
	BaseAnimator* m_animator;
#endif
	// Smaller vars last to improve alignment and/or decrease structure size
	bool m_isActive;
	bool movingLeft;
	bool movingRight;
	bool jumping;
	bool animate;
	bool movable; // What do we need this for? // Dunno, did I put this here? :o
	bool changing; // This shouldnt be in the worm class ( its player stuff >:O )
	int m_dir;
};

#endif  // _WORM_H_
