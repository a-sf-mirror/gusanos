#ifndef WEAPON_H
#define WEAPON_H

class BaseWorm;
class WeaponType;

class Weapon
{
public:
	
	friend class BaseWorm;

	enum Actions
	{
		PRIMARY_TRIGGER,
		SECONDARY_TRIGGER
	};
		
	Weapon(WeaponType* type, BaseWorm* owner);
	~Weapon();
	
	void think();
	
	void actionStart( Actions action );
	void actionStop( Actions action );
	
	void delay( int time );
	BaseWorm* getOwner();

private:
		
	bool primaryShooting;
	int ammo;
	int inactiveTime;
	
	WeaponType* m_type;
	BaseWorm* m_owner;
};

#endif  // _WEAPON_H_
