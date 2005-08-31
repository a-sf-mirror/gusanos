#ifndef WEAPON_H
#define WEAPON_H

#include <zoidcom.h>
#include <stddef.h>

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
	
	enum Events
	{
		RELOADED,
		SHOOT,
		OUTOFAMMO
	};
		
	Weapon(WeaponType* type, BaseWorm* owner);
	~Weapon();
	
	void think( bool isFocused, size_t index );
	
	void reset();
	
	void actionStart( Actions action );
	void actionStop( Actions action );
	void recieveMessage( ZCom_BitStream* data );
	
	void delay( int time );
	BaseWorm* getOwner();
	
	
	bool reloading;

private:
	
	void reload();
	void outOfAmmo();
		
	bool primaryShooting;
	int ammo;
	int inactiveTime;
	int reloadTime;
	
	WeaponType* m_type;
	BaseWorm* m_owner;
};

#endif  // _WEAPON_H_
