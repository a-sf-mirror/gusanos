#ifndef WEAPON_H
#define WEAPON_H


class Worm;
class WeaponType;

class Weapon
{
	public:

	enum Actions
	{
		PRIMARY_TRIGGER,
		SECONDARY_TRIGGER
	};
		
	Weapon(WeaponType* type, Worm* owner);
	~Weapon();
	
	void think();
	
	void actionStart( Actions action );
	void actionStop( Actions action );
	
	void delay( int time );
	Worm* getOwner();

	private:
		
	bool primaryShooting;
	int ammo;
	int inactiveTime;
	
	WeaponType* m_type;
	Worm* m_owner;
};

#endif  // _WEAPON_H_
