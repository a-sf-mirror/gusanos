#include "weapon_type.h"

#include "events.h"
#include "sprite_set.h"
#include "text.h"
#include "parser.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

WeaponType::WeaponType()
{
	ammo = 1;
	reloadTime = 0;
	laserSightIntensity = 0;
	laserSightRange = 0;

	firecone = NULL;

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

bool WeaponType::load(const string &filename)
{
	//cerr << "Loading weapon: " << filename << endl;
	fileName = filename;
	ifstream fileStream(filename.c_str());
	
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
					else if ( var == "laser_sight_range" ) laserSightRange = cast<float>(val);
					else if ( var == "firecone" ) firecone = spriteList.load(val);
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
						currEvent = new Event;
						primaryShoot = currEvent;
					}
					else if ( eventName == "primary_press" )
					{
						currEvent = new Event;
						primaryPressed = currEvent;
					}
					else if ( eventName == "primary_release" )
					{
						currEvent = new Event;
						primaryReleased = currEvent;
					}
					else if ( eventName == "out_of_ammo" )
					{
						currEvent = new Event;
						outOfAmmo = currEvent;
					}
					else if ( eventName == "reaload_end" )
					{
						currEvent = new Event;
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
					currEvent->addAction(*iter, Parser::getActionParams( tokens ));
				}
				
			}
		}
		fileStream.close();
		return true;
	} else
	{
		return false;
	}
}


