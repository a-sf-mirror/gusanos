// Liero weapon/object/special object loading
// WARNING: This code sucks and is old..  its being revamped though ;)
// - nym

#ifndef __LIERO_H__
#define __LIERO_H__

#include <iostream>

/* Liero binary offsets */
#define WEAPON_COUNT	40
#define WEAPON_OFFSET	112806
#define WEAPON_NAME_OFFSET	0x1B675
#define OBJECT_COUNT	24
#define OBJECT_OFFSET	111430
#define SOBJECT_COUNT	13
#define SOBJECT_OFFSET	115218


typedef unsigned char byte;

namespace Liero
{

/* Map related */
const int MAP_WIDTH = 504;
const int MAP_HEIGHT = 350;

/* Liero Weapon Structure */
struct LieroWeapons
{
	byte wormDetectRange[WEAPON_COUNT];
	bool wormAffect[WEAPON_COUNT];
	byte blow[WEAPON_COUNT];
	byte order[WEAPON_COUNT];			//??? LK ???
	short gravity[WEAPON_COUNT];
	bool shadow[WEAPON_COUNT];
	bool laserSight[WEAPON_COUNT];
	byte soundLaunch[WEAPON_COUNT];
	byte soundLoop[WEAPON_COUNT];
	byte soundExplode[WEAPON_COUNT];
	short speed[WEAPON_COUNT];
	short accAdd[WEAPON_COUNT];
	short distribution[WEAPON_COUNT];
	byte numObjects[WEAPON_COUNT];
	byte recoil[WEAPON_COUNT];
	short accMultiply[WEAPON_COUNT];
	short shotDelay[WEAPON_COUNT];
	short loadingTime[WEAPON_COUNT];
	byte ammo[WEAPON_COUNT];
	byte createOnHit[WEAPON_COUNT];
	byte groundAffect[WEAPON_COUNT];
	byte leaveShells[WEAPON_COUNT];
	byte shellDelay[WEAPON_COUNT];
	bool playReloadSound[WEAPON_COUNT];		//??? LK ???
	bool wormExplode[WEAPON_COUNT];
	bool groundExplode[WEAPON_COUNT];
	byte wormCollide[WEAPON_COUNT];			//??? LK ???
	byte fireCone[WEAPON_COUNT];
	bool objectCollide[WEAPON_COUNT];
	bool explodeAffect[WEAPON_COUNT];
	byte bounce[WEAPON_COUNT];
	short timeToExplode[WEAPON_COUNT];
	short timeToExplodeV[WEAPON_COUNT];
	byte hitDamage[WEAPON_COUNT];
	byte blood[WEAPON_COUNT];
	short animSFrame[WEAPON_COUNT];
	byte animFrames[WEAPON_COUNT];
	bool animLoop[WEAPON_COUNT];
	byte shotType[WEAPON_COUNT];			//??? LK ???
	byte bulletColor[WEAPON_COUNT];
	byte splinterAmmount[WEAPON_COUNT];
	byte splinterColor[WEAPON_COUNT];
	byte splinterType[WEAPON_COUNT];
	byte splinterScatter[WEAPON_COUNT];		//??? LK ???
	byte oTrailType[WEAPON_COUNT];
	byte oTrailDelay[WEAPON_COUNT];
	byte pTrailType[WEAPON_COUNT];
	byte pTrailPType[WEAPON_COUNT];
	byte pTrailDelay[WEAPON_COUNT];
};

/* Liero Weapon Name Structure */
struct LieroWeaponNames
{
	char data[40][14];
};

/* Liero object Structure */
struct LieroObjects
{
	byte wormDetectRange[OBJECT_COUNT];
	short gravity[OBJECT_COUNT];
	short speed[OBJECT_COUNT];
	short speedV[OBJECT_COUNT];			//??? LK ???
	short distribution[OBJECT_COUNT];
	char blow[OBJECT_COUNT];
	byte bounce[OBJECT_COUNT];
	char hitDamage[OBJECT_COUNT];
	bool wormExplode[OBJECT_COUNT];
	bool groundExplode[OBJECT_COUNT];
	bool wormDestroy[OBJECT_COUNT];			//??? LK ???
	byte blood[OBJECT_COUNT];
	byte animSFrame[OBJECT_COUNT];
	byte animFrames[OBJECT_COUNT];
	bool drawOnMap[OBJECT_COUNT];			//??? LK ???
	byte bulletColor[OBJECT_COUNT];
	short createOnHit[OBJECT_COUNT];
	bool explodeAffect[OBJECT_COUNT];
	char dirtAffect[OBJECT_COUNT];			//??? LK ???
	byte splinterAmmount[OBJECT_COUNT];
	byte splinterColor[OBJECT_COUNT];
	short splinterType[OBJECT_COUNT];
	bool bTrail[OBJECT_COUNT];
	byte bTrailDelay[OBJECT_COUNT];
	short leaveObj[OBJECT_COUNT];			//??? LK ???
	byte leaveObjDelay[OBJECT_COUNT];		//??? LK ???
	long timeToExplode[OBJECT_COUNT];
	long timeToExplodeV[OBJECT_COUNT];
};

/* Liero Special Object Structure */
struct LieroSObjects
{
	byte startSound[SOBJECT_COUNT];			//??? LK ???
	byte numSounds[SOBJECT_COUNT];			//??? LK ???
	byte animDelay[SOBJECT_COUNT];			//??? LK ???
	byte animSFrame[SOBJECT_COUNT];
	byte animFrames[SOBJECT_COUNT];
	byte wormDetectRange[SOBJECT_COUNT];
	byte hitDamage[SOBJECT_COUNT];
	short blow[SOBJECT_COUNT];
	byte shadow[SOBJECT_COUNT];			//??? LK ???
	byte shake[SOBJECT_COUNT];			//??? LK ???
	byte flash[SOBJECT_COUNT];			//??? LK ???
	char dirtAffect[SOBJECT_COUNT];			//??? LK ???
};

/* Liero level structure */
struct LieroLevel
{
	byte level[MAP_HEIGHT][MAP_WIDTH];
	byte material[MAP_HEIGHT][MAP_WIDTH];
};

/* typedef SDL_Color LieroColor; */
struct LieroColor
{
	byte r;
	byte g;
	byte b;
	byte unused;
};

#if 0
/* Load weapons from liero executable */
bool loadWeapons(LieroWeapons* lw, const std::string &lieroBin);
/* Load weapon names from liero executable */
bool loadWeaponNames(LieroWeaponNames* lwn, char* lieroBin);
/* Load singular weapon from liero executable */
bool loadWeapon(LieroWeapons* weapon, LieroWeaponNames* weaponNames, unsigned int i, char* weapFile);
/* Output to LieroKit compatible weapon to stdout */
bool outputWeapon(LieroWeapons* lw, LieroWeaponNames* lwn, unsigned int i);

/* Load objects from liero binary */
bool loadObjects(LieroObjects* lo, char* file);
/* Load special objects from liero binary */
bool loadSObjects(LieroSObjects* lso, char* file);

#endif

/* Liero Palette Loader */
bool loadPalette(LieroColor palette[], const std::string &palFile);
/* Liero level loader */
bool loadLevel(LieroLevel *level, LieroColor palette[], const std::string &lvlFile);

} // end namespace Liero

#endif
