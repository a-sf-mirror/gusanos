#ifndef WEAPONS_H
#define WEAPONS_H

#include <ctype.h>
#include <allegro.h>
#ifdef WINDOWS
#include "winalleg.h"
#endif
#include <time.h>
#include "engine.h"

#include <iostream>
#include <fstream>
#include <string>

using std::ifstream;

//forward
class sound;

class weapon
{
  public:
  weapon();
  ~weapon();

	int shoot_spd,shoot_num,distribution,shoot_times,shoot_spd_rnd,aim_recoil,recoil,affected_motion;
  int firecone_timeout,ammo,reload_time,lsight_intensity,lsight_fade,start_delay;
  char autofire;
	struct part_type *shoot_obj,*create_on_release;
  sprite *firecone;
	sound *shoot_sound,*reload_sound,*noammo_sound,*start_sound;
	char name[512],filename[512];
	class weapon *next,*prev;
};

class weap_list
{
  public:
	class weapon *start,*end;
  class weapon *num[100];
  int weap_count;
	weap_list();
  ~weap_list();
	void destroy();
};

extern class weap_list *weaps;

class weapon* load_weap(const char* weap_name);
void scanWeapsDir();

#endif /* WEAPONS_H */
