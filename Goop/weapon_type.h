#ifndef PART_TYPE_H
#define PART_TYPE_H

#include "events.h"

#include <string>
#include <vector>

class Sprite;

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

   Sprite *firecone;
	std::string name;

	Event *primaryShoot;
	Event *primaryPressed;
	Event *primaryReleased;
	Event *outOfAmmo;
};

#endif // _PART_TYPE_H_
