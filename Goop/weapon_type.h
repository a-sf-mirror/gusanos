#ifndef WEAPON_TYPE_H
#define WEAPON_TYPE_H

//#include "events.h"

#include <string>
#include <vector>

class SpriteSet;
class Event;

class WeaponType
{
	public:
		
	WeaponType();
	~WeaponType();
	
	bool load(const std::string &filename);
	
	int ammo;
	int reloadTime;
	float laserSightIntensity;
	float laserSightRange;

 	SpriteSet *firecone;
	std::string name;
	std::string fileName;

	Event *primaryShoot;
	Event *primaryPressed;
	Event *primaryReleased;
	Event *outOfAmmo;
	Event *reloadEnd;
};

class WeaponOrder
{
	public:
	bool operator () ( WeaponType* weap1, WeaponType* weap2)
	{
		if ( weap1->fileName < weap2->fileName )
			return true;
		return false;
	}
};

#endif // _WEAPON_TYPE_H_
