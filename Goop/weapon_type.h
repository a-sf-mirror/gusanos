#ifndef PART_TYPE_H
#define PART_TYPE_H

#include "resource_list.h"
#include "part_events.h"
#include "distortion.h"

#include <allegro.h>
#include <string>
#include <vector>

class Sprite;
class WeaponEvent;

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

	WeaponEvent *primaryShoot;
	WeaponEvent *primaryPressed;
	WeaponEvent *primaryReleased;
	WeaponEvent *outOfAmmo;
}

extern ResourceList<PartType> partTypeList;

#endif // _PART_TYPE_H_
