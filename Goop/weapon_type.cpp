#include "weapon_type.h"

#include "events.h"
#include "sprite_set.h"
#include "omfgutil_text.h"
#include "parser.h"
#include "resource_base.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
namespace fs = boost::filesystem;

using namespace std;

WeaponType::WeaponType() : ResourceBase()
{
	ammo = 1;
	reloadTime = 0;
#ifndef DEDSERV
	firecone = NULL;
#endif
	
	laserSightColour = makecol(255,0,0);
	laserSightRange = -1;
	laserSightIntensity = 0;
	laserSightAlpha = 255;
	laserSightBlender = NONE;

	primaryShoot = NULL;
	primaryPressed = NULL;
	primaryReleased = NULL;
	outOfAmmo = NULL;
	reloadEnd = NULL;
}

WeaponType::~WeaponType()
{
	delete primaryShoot;
	delete primaryPressed;
	delete primaryReleased;
	delete outOfAmmo;
	delete reloadEnd;
}

bool WeaponType::load(const fs::path &filename)
{
	//cerr << "Loading weapon: " << filename << endl;
	fileName = filename;
	fs::ifstream fileStream(filename);
	
	if ( fileStream )
	{
		string parseLine;
		Event *currEvent = NULL;
		while ( portable_getline( fileStream, parseLine ) )
		{
			//if ( !parseLine.empty() )
			{
				string var;
				string val;

				vector<string> tokens;
				tokens = Parser::tokenize ( parseLine );
				int lineID = Parser::identifyLine( tokens );
				
				vector<string>::iterator iter = tokens.begin();
				
				if ( lineID == Parser::PROP_ASSIGMENT )
				{
					var = *iter;
					iter++;
					if ( iter != tokens.end() && *iter == "=")
					{
						iter++;
						if ( iter != tokens.end() )
							val = *iter;
					}
					
					if ( var == "ammo" ) ammo = cast<int>(val);
					else if ( var == "name" ) name = val;
					else if ( var == "reload_time" ) reloadTime = cast<int>(val);
					else if ( var == "laser_sight_intensity" ) laserSightIntensity = cast<float>(val);
					else if ( var == "laser_sight_range" ) laserSightRange = cast<int>(val);
					else if ( var == "laser_sight_alpha" ) laserSightAlpha = cast<int>(val);
					else if ( var == "laser_sight_blender" )
					{
						if ( val == "add" ) laserSightBlender = ADD;
						else if ( val == "alpha" ) laserSightBlender = ALPHA;
					}
					else if ( var == "laser_sight_colour" || var == "laser_sight_color" )
					{
						if ( tokens.size() >= 5 )
							laserSightColour = makecol( cast<int>(tokens[2]), cast<int>(tokens[3]), cast<int>(tokens[4]) );
					}
#ifndef DEDSERV
					else if ( var == "firecone" ) firecone = spriteList.load(val);
#else
					else if ( var == "firecone" ) /* ignore */;
#endif
					else
					{
						std::cout << "Unknown variable on following line:" << std::endl;
						std::cout << "\t" << parseLine << std::endl;
					}
				}
				
				if ( lineID == Parser::EVENT_START )
				{
					iter++;
					string eventName = *iter;
					if ( eventName == "primary_shoot" )
					{
						currEvent = new Event(Event::ProvidesWeapon | Event::ProvidesObject);
						primaryShoot = currEvent;
					}
					else if ( eventName == "primary_press" )
					{
						currEvent = new Event(Event::ProvidesWeapon | Event::ProvidesObject);
						primaryPressed = currEvent;
					}
					else if ( eventName == "primary_release" )
					{
						currEvent = new Event(Event::ProvidesWeapon | Event::ProvidesObject);
						primaryReleased = currEvent;
					}
					else if ( eventName == "out_of_ammo" )
					{
						currEvent = new Event(Event::ProvidesWeapon | Event::ProvidesObject);
						outOfAmmo = currEvent;
					}
					else if ( eventName == "reaload_end" )
					{
						currEvent = new Event(Event::ProvidesWeapon | Event::ProvidesObject);
						reloadEnd = currEvent;
					}
					else
					{
						std::cout << "Unknown event on following line:" << std::endl;
						std::cout << "\t" << parseLine << std::endl;
						std::cout << "Event name given: \"" << eventName << "\"" << std::endl;
						std::cout << "----------------" << std::endl;
						currEvent = NULL;
					}
				}
				
				if ( lineID == Parser::ACTION && currEvent != NULL)
				{
					if(!currEvent->addAction(*iter, Parser::getActionParams( tokens )))
					{
						//TODO: Add more info here
						cerr << "Couldn't add action to event" << endl;
					}
				}
				
			}
		}
		
		return true;
		
	} else
	{
		return false;
	}
}


