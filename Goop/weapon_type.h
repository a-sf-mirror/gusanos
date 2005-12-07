#ifndef WEAPON_TYPE_H
#define WEAPON_TYPE_H

#include "gfx.h"
#include "resource_base.h"

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>
namespace fs = boost::filesystem;

#ifndef DEDSERV
class SpriteSet;
#endif
class Event;
class TimerEvent;

class WeaponType : public ResourceBase
{
	public:
		
	WeaponType();
	~WeaponType();
	
	bool load(const fs::path &filename);

	int ammo;
	int reloadTime;
	
	int laserSightColour;
	int laserSightRange;
	float laserSightIntensity;
	int laserSightAlpha;
	Blenders laserSightBlender; // Change to BlitterContext::Type
	
#ifndef DEDSERV
	SpriteSet *firecone;
#endif
	std::string name;
	fs::path fileName;

	std::vector< TimerEvent* > timer;
	std::vector< TimerEvent* > activeTimer;
	std::vector< TimerEvent* > shootTimer;

	Event *primaryShoot;
	Event *primaryPressed;
	Event *primaryReleased;
	Event *outOfAmmo;
	Event *reloadEnd;
};

struct WeaponOrder
{
	bool operator () ( WeaponType* weap1, WeaponType* weap2)
	{
		if ( weap1->fileName.leaf() < weap2->fileName.leaf() )
			return true;
		return false;
	}
};

#endif // _WEAPON_TYPE_H_
