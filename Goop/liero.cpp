// Liero weapon/object/special object loading
// WARNING: This code sucks and is old..  its being revamped though ;)
// - nym

#include "liero.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace Liero
{

LieroMaterial defaultMaterials[5][32] = 
{
	//Dirt
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//Dirt 2
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//Rock
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//Background
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	//Shadow
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

#if 0
/*
 * Load weapons from Liero binary
 */
void loadWeapons(LieroWeapons* lw, char* lieroBin)
{
  FILE* fp;
  fp = fopen(lieroBin, "rb");
  fseek(fp, WEAPON_OFFSET, SEEK_SET);
  fread(lw, sizeof(LieroWeapons), 1, fp);
  if (ferror(fp))
    {
      fprintf(stderr, "ERROR: Reading weapons from liero binary.\n");
      exit(1);
    }
  fclose(fp);
}

/*
 * Load weapon names from liero binary
 */
void loadWeaponNames(LieroWeaponNames* lwn, char* lieroBin)
{
  FILE* fp;
  fp = fopen(lieroBin, "rb");
  fseek(fp, WEAPON_NAME_OFFSET, SEEK_SET);
  for (int i = 0; i < WEAPON_COUNT; i++)
    {
      byte len;
      fread(lwn, 1, 1, fp);
      fread(&lwn->data[i], 13, 1, fp);
      lwn->data[i][len] = 0;
      if (ferror(fp))
	{
	  printf("Error reading weapon names.\n");
	  exit(1);
	}
    }
  fclose(fp);
}

/*
 * Load LieroKit weapon (.lwp)
 */
void loadWeapon(LieroWeapons* weapons, LieroWeaponNames* weaponNames, int i, char* weapFile)
{
    FILE* fp;
    
    fp = fopen(weapFile, "r");
    
    if (ferror(fp))
    {
        fprintf(stderr, "ERROR: fopen(\"%s\", \"r\")\n", weapFile);
        exit(1);
    }
    
    char buffer[256];
    fgets(buffer, 256, fp);
    if (strncmp(buffer, "LIEROKIT:WEAPONPLUGIN", 21) != 0)
    {
        fprintf(stderr, "ERROR: %s not LieroKit weapon\n", weapFile);
        exit(1);
    }
    
    while (fgets(buffer, 256, fp) != NULL)
    {
        //Scan string for ':' delimeter
        int l;
        for (int j = 0; j < 256; j++)
        {
            if (buffer[j] == ':')
            {
                l = j - 1;
                break;
            }
        }
		
		if (strncmp(buffer, "NAME", l) == 0)
		{
			strcpy(&weaponNames->data[i][0], &buffer[l + 2]);
		} else
		if (strncmp(buffer, "WORMDETECTRANGE", l) == 0)
		{
			weapons->wormDetectRange[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "WORMAFFECT", l) == 0)
		{
			weapons->wormAffect[i] = (bool)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "BLOW", l) == 0)
		{
			weapons->blow[i] = (byte)atoi(&buffer[i + 2]);
		} else
		if (strncmp(buffer, "ORDER", l) == 0)
		{
			/* Unsure about this one */
		} else
		if (strncmp(buffer, "GRAVITY", l) == 0)
		{
			weapons->gravity[i] = (short)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "SHADOW", l) == 0)
		{
			weapons->shadow[i] = (bool)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "LASERSIGHT", l) == 0)
		{
			weapons->laserSight[i] = (bool)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "SOUNDLAUNCH", l) == 0)
		{
			weapons->soundLaunch[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "SOUNDLOOP", l) == 0)
		{
			weapons->soundLoop[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "SOUNDEXPLODE", l) == 0)
		{
			weapons->soundExplode[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "SPEED", l) == 0)
		{
			weapons->speed[i] = (short)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "ACCADD", l) == 0)
		{
			weapons->accAdd[i] = (short)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "DISTRIBUTION", l) == 0)
		{
			weapons->distribution[i] = (short)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "NUMOBJECTS", l) == 0)
		{
			weapons->distribution[i] = (short)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "RECOIL", l) == 0)
		{
			weapons->recoil[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "ACCMULTIPLY", l) == 0)
		{
			weapons->accMultiply[i] = (short)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "SHOTDELAY", l) == 0)
		{
			weapons->shotDelay[i] = (short)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "LOADINGTIME", l) == 0)
		{
			weapons->loadingTime[i] = (short)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "AMMO", l) == 0)
		{
			weapons->ammo[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "CREATEONHIT", l) == 0)
		{
			weapons->createOnHit[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "GROUNDAFFECT", l) == 0)
		{
			weapons->groundAffect[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "LEAVESHELLS", l) == 0)
		{
			weapons->leaveShells[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "SHELLDELAY", l) == 0)
		{
			weapons->shellDelay[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "PLAYRELOADSOUND", l) == 0)
		{
			/* Not sure about this one */
		} else
		if (strncmp(buffer, "WORMEXPLODE", l) == 0)
		{
			weapons->wormExplode[i] = (bool)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "GROUNDEXPLODE", l) == 0)
		{
			weapons->groundExplode[i] = (bool)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "WORMCOLLIDE", l) == 0)
		{
			weapons->wormCollide[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "FIRECONE", l) == 0)
		{
			weapons->fireCone[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "OBJECTCOLLIDE", l) == 0)
		{
			weapons->objectCollide[i] = (bool)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "EXPLODEAFFECT", l) == 0)
		{
			weapons->explodeAffect[i] = (bool)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "BOUNCE", l) == 0)
		{
			weapons->bounce[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "TIMETOEXPLODE", l) == 0)
		{
			weapons->timeToExplode[i] = (short)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "TIMETOEXLODEV", l) == 0)
		{
			weapons->timeToExplodeV[i] = (short)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "HITDAMAGE", l) == 0)
		{
			weapons->hitDamage[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "BLOOD", l) == 0)
		{
			weapons->hitDamage[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "ANIMSFRAME", l) == 0)
		{
			weapons->animSFrame[i] = (short)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "ANIMFRAMES", l) == 0)
		{
			weapons->animFrames[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "ANIMLOOP", l) == 0)
		{
			weapons->animLoop[i] = (bool)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "SHOTTYPE", l) == 0)
		{
			weapons->shotType[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "BULLETCOLOR", l) == 0)
		{
			weapons->bulletColor[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "SPLINTERAMMOUNT", l) == 0)
		{
			weapons->splinterAmmount[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "SPLINTERCOLOR", l) == 0)
		{
			weapons->splinterColor[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "SPLINTERSCATTER", l) == 0)
		{
			weapons->splinterScatter[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "OTRAILTYPE", l) == 0)
		{
			weapons->oTrailType[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "OTRAILDELAY", l) == 0)
		{
			weapons->oTrailDelay[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "PTRAILTYPE", l) == 0)
		{
			weapons->pTrailPType[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "PTRAILPTYPE", l) == 0)
		{
			weapons->pTrailPType[i] = (byte)atoi(&buffer[l + 2]);
		} else
		if (strncmp(buffer, "PTRAILDELAY", l) == 0)
		{
			weapons->pTrailDelay[i] = (byte)atoi(&buffer[l + 2]);
		}
    }
    
    fclose(fp);
}

/*
 * Output liero weapon to stdout
 */
void outputWeapon(LieroWeapons* lw, LieroWeaponNames* lwn, unsigned int i)
{
	printf("LIEROKIT:WEAPONPLUGIN\n");
	printf("NAME:");
	for (int c = 0; c < 14; c++)
	{
		printf("%c", lwn->data[i][c]);
	}
	printf("\n");
	
	printf("WORMDETECTRANGE:%i\n", lw->wormDetectRange[i]);
	printf("WORMAFFECT:%i\n", lw->wormAffect[i]);
	printf("BLOW:%i\n", lw->blow[i]);
	printf("__ORDER:%i\n", lw->order[i]);
	printf("GRAVITY:%i\n", lw->gravity[i]);
	printf("SHADOW:%i\n", lw->shadow[i]);
	printf("LASERSIGHT:%i\n", lw->laserSight[i]);
	printf("SOUNDLAUNCH:%i\n", lw->soundLaunch[i]);
	printf("SOUNDLOOP:%i\n", lw->soundLoop[i]);
	printf("SOUNDEXPLODE:%i\n", lw->soundExplode[i]);
	printf("SPEED:%i\n", lw->speed[i]);
	printf("ACCADD:%i\n", lw->accAdd[i]);
	printf("DISTRIBUTION:%i\n", lw->distribution[i]);
	printf("NUMOBJECTS:%i\n", lw->numObjects[i]);
	printf("RECOIL:%i\n", lw->recoil[i]);
	printf("ACCMULTIPLY:%i\n", lw->accMultiply[i]);
	printf("SHOTDELAY:%i\n", lw->shotDelay[i]);
	printf("LOADINGTIME:%i\n", lw->loadingTime[i]);
	printf("AMMO:%i\n", lw->ammo[i]);
	printf("CREATEONHIT:%i\n", lw->createOnHit[i]);
	printf("GROUNDAFFECT:%i\n", lw->groundAffect[i]);
	printf("LEAVESHELLS:%i\n", lw->leaveShells[i]);
	printf("SHELLDELAY:%i\n", lw->shellDelay[i]);
	printf("__PLAYRELOADSOUND:%i\n", lw->playReloadSound[i]);
	printf("WORMEXPLODE:%i\n", lw->wormExplode[i]);
	printf("GROUNDEXPLODE:%i\n", lw->groundExplode[i]);
	printf("__WORMCOLLIDE:%i\n", lw->wormCollide[i]);
	printf("FIRECONE:%i\n", lw->fireCone[i]);
	printf("OBJECTCOLLIDE:%i\n", lw->objectCollide[i]);
	printf("EXPLODEAFFECT:%i\n", lw->explodeAffect[i]);
	printf("BOUNCE:%i\n", lw->bounce[i]);
	printf("TIMETOEXPLODE:%i\n", lw->timeToExplode[i]);
	printf("TIMETOEXPLODEV:%i\n", lw->timeToExplodeV[i]);
	printf("HITDAMAGE:%i\n", lw->hitDamage[i]);
	printf("BLOOD:%i\n", lw->blood[i]);
	printf("ANIMSFRAME:%i\n", lw->animSFrame[i]);
	printf("ANIMFRAMES:%i\n", lw->animFrames[i]);
	printf("ANIMLOOP:%i\n", lw->animLoop[i]);
	printf("SHOTTYPE:%i\n", lw->shotType[i]);
	printf("BULLETCOLOR:%i\n", lw->bulletColor[i]);
	printf("SPLINTERAMMOUNT:%i\n", lw->splinterAmmount[i]);
	printf("SPLINTERCOLOR:%i\n", lw->splinterColor[i]);
	printf("SPLINTERTYPE:%i\n", lw->splinterType[i]);
	printf("__SPLINTERSCATTER:%i\n", lw->splinterScatter[i]);
	printf("OTRAILTYPE:%i\n", lw->oTrailType[i]);
	printf("OTRAILDELAY:%i\n", lw->oTrailDelay[i]);
	printf("PTRAILTYPE:%i\n", lw->pTrailType[i]);
	printf("PTRAILPTYPE:%i\n", lw->pTrailPType[i]);
	printf("PTRAILDELAY:%i\n", lw->pTrailDelay[i]);
}

/* Read liero objects */
void loadObjects(LieroObjects* lo, char *file)
{
  FILE *fp;
  fp = fopen(file, "rb");

  fseek(fp, OBJECT_OFFSET, SEEK_SET);
  fread(lo, sizeof(LieroObjects), 1, fp);
  if (ferror(fp))
    {
      printf("Error reading objects.\n");
      exit(1);
    }

  fclose(fp);
}

/* Read special objects */
void loadSObjects(LieroSObjects* lso, char *file)
{
  FILE* fp;
  fp = fopen(file, "rb");

  fseek(fp, SOBJECT_OFFSET, SEEK_SET);
  fread(lso, sizeof(LieroSObjects), 1, fp);
  if (ferror(fp))
    {
      printf("Error reading special objects.\n");
      exit(1);
    }

  fclose(fp);
}

#endif

/* Load liero palette (.lpl) */
bool loadPalette(LieroColor* palette, const std::string &palFile)
{
	std::ifstream fin;
	fin.open(palFile.c_str());
    
	if (!fin.is_open())
		return false;
    
 	long size;
	fin.seekg(0, std::ios::beg);
	size = fin.tellg();
	fin.seekg(0, std::ios::end);
	size =  fin.tellg() - size;
	fin.seekg(0, std::ios::beg);

	if (size != 768)
		return false;

	for (int i = 0; i < 256; i++)
	{
		fin.read((char*)&palette[i], 3);
	}
	fin.close();
	return true;
}


/* Load liero level */
bool loadLevel(LieroLevel *level, LieroColor palette[], const std::string &lvlFile)
{
	std::ifstream fin;
	fin.open(lvlFile.c_str(), std::ios::binary);
	if (!fin.is_open())
		return false;

	long size;
	fin.seekg(0, std::ios::beg);
	size = fin.tellg();
	fin.seekg(0, std::ios::end);
	size =  fin.tellg() - size;
	fin.seekg(0, std::ios::beg);

	std::cout << "Level: " << size << " bytes. ";
	//Make sure right size (normal level or bigger)
	if (size < 176400)
	{
		return 0;
	}

	//load level
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			byte color;
			fin.read((char*)&color, 1);
			level->level[y][x] = color;
		}
	}

	//check for powerlevel
	if (size > 176400)
	{
		char pwrlvl[10];
		fin.read((char*)pwrlvl, 10);

		//powerlevel sizes
		if (size == 177178 || size == 177338)
		{
			if (strncmp(pwrlvl, "POWERLEVEL", 10) == 0)
			{
				std::cout << "Powerlevel";
				//load pal
				for (int i = 0; i < 256; i++)
				{
					byte color[3];
					fin.read((char*)color, 3);
					palette[i].r = color[0] << 2;
					palette[i].g = color[1] << 2;
					palette[i].b = color[2] << 2;
				}
			} else
			if (strncmp(pwrlvl, "POWERLEVL2", 10) == 0)
			{
				std::cout << "Unofficial Powerlevel 2";
				//load pal
				for (int i = 0; i < 256; i++)
				{
					byte color[3];
					fin.read((char*)color, 3);
					palette[i].r = color[0] << 2;
					palette[i].g = color[1] << 2;
					palette[i].b = color[2] << 2;
				}

				//load materials
				//fin.read((char*)&level->materials[0], 32 * 5);
			} else
			if (strncmp(pwrlvl, "PL2 ", 4) == 0)
			{
				std::cout << "Powerlevel 2";
				for (int i = 10; i >= 4; --i)
					fin.putback(pwrlvl[i]);
				dword version;
				fin.read((char*)&version, sizeof(dword));

				if (version > 0x00010000)
				{
					dword flags;
					fin.read((char*)&flags, sizeof(flags));

					if (flags & PL2_PALETTE)
					{
						for (int i = 0; i < 256; i++)
						{
							byte color[3];
							fin.read((char*)color, 3);
							palette[i].r = color[0] << 2;
							palette[i].g = color[1] << 2;
							palette[i].b = color[2] << 2;
						}
					}

					if (flags & PL2_MATERIALS)
					{
						fin.read((char*)level->materials, 32 * 5);
					} else
					{
						for (int i = 0; i < 32 * 5; i++)
						{
							level->materials[0][i] = defaultMaterials[0][i];
						}
					}

					if (flags & PL2_SETTINGS)
					{
						//add level settings
						fin.seekg(sizeof(LieroRect), std::ios::cur);
						if (version >= 0x00030000)
							fin.seekg(sizeof(LieroColorRange) * 4 + 4, std::ios::cur);
						else
							fin.seekg(sizeof(LieroColorRange) * 4 + 4, std::ios::cur);

						if (version == 0x00040000)
							fin.seekg(sizeof(dword) + sizeof(LieroRect) + 1, std::ios::cur);
						else
							fin.seekg(sizeof(dword) + sizeof(LieroRect) + 1, std::ios::cur);
					}

					if (flags & PL2_BACKGROUND)
					{
						//add background
						fin.read((char*) level->background, 16 * 16 * 2);
					} else
						for (int i = 0; i < 16 * 16 * 2; i++)
							level->background[0][0][i] = (rand() % 4) + 160;
				}
			}
		}
	}

	if (fin.fail())
	{
		fin.close();
		return false;
	}

	std::cout << std::endl;

	fin.close();
	return true;
}

} // end namespace Lieor

