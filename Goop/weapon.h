#ifndef WEAPON_H
#define WEAPON_H

#include <zoidcom.h>
//#include <cstddef>

class BITMAP;
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
		RELOADED = 0,
		//SHOOT, //unused?
		OUTOFAMMO,
		EventsCount,
	};
		
	Weapon(WeaponType* type, BaseWorm* owner);
	~Weapon();
	
	void think( bool isFocused, size_t index );
	
	void reset();
	
#ifndef DEDSERV
	void drawBottom(BITMAP* where,int x, int y);
	void drawTop(BITMAP* where,int x, int y)
	{ }
#endif
	
	void actionStart( Actions action );
	void actionStop( Actions action );
	void recieveMessage( ZCom_BitStream* data );
	
	void delay( int time );
	BaseWorm* getOwner();
	
	WeaponType* getType() { return m_type; }
	
	
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
