#include "weapon_type.h"

#include "events.h"
#include "sprite_set.h"
#include "util/text.h"
#include "parser.h"
#include "resource_base.h"
#include "game_actions.h"
#include "omfg_script.h"

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


namespace EventID
{
enum type
{
	PrimaryShoot,
	PrimaryPress,
	PrimaryRelease,
	OutOfAmmo,
	ReloadEnd,
};
}

bool WeaponType::load(fs::path const& filename)
{
	fs::ifstream fileStream(filename);

	if (!fileStream )
		return false;
	
	OmfgScript::Parser parser(fileStream, gameActions, filename.native_file_string());
	
	namespace af = OmfgScript::ActionParamFlags;
		
	parser.addEvent("primary_shoot", EventID::PrimaryShoot, af::Weapon | af::Object);
	parser.addEvent("primary_press", EventID::PrimaryPress, af::Weapon | af::Object);
	parser.addEvent("primary_release", EventID::PrimaryRelease, af::Weapon | af::Object);
	parser.addEvent("out_of_ammo", EventID::OutOfAmmo, af::Weapon | af::Object);
	parser.addEvent("reload_end", EventID::ReloadEnd, af::Weapon | af::Object);
	
	if(!parser.run())
	{
		if(parser.incomplete())
			parser.error("Trailing garbage");
		return false;
	}

	ammo = parser.getInt("ammo", 1);
	name = parser.getString("name");
	reloadTime = parser.getInt("reload_time", 0);
	laserSightIntensity = parser.getDouble("laser_sight_intensity", 0.0);
	laserSightRange = parser.getInt("laser_sight_range", -1);
	laserSightAlpha = parser.getInt("laser_sight_alpha", 255);
	{
		std::string const& str = parser.getString("laser_sight_blender", "none");
		if(str == "add") laserSightBlender = ADD;
		else if(str == "alpha") laserSightBlender = ALPHA;
		else if(str == "none") laserSightBlender = NONE;
	}

	laserSightColour = parser.getProperty("laser_sight_colour", "laser_sight_color")->toColor(255, 0, 0);
#ifndef DEDSERV
	{
		OmfgScript::TokenBase* v = parser.getProperty("firecone");
		if(!v->isDefault())
			firecone = spriteList.load(v->toString());
	}
#endif
	
	OmfgScript::Parser::EventIter i(parser);
	for(; i; ++i)
	{
		std::vector<OmfgScript::TokenBase*> const& p = i.params();
		switch(i.type())
		{
			case EventID::PrimaryShoot:
				primaryShoot = new Event(i.actions());
			break;
			
			case EventID::PrimaryPress:
				primaryPressed = new Event(i.actions());
			break;
			
			case EventID::PrimaryRelease:
				primaryReleased = new Event(i.actions());
			break;
			
			case EventID::OutOfAmmo:
				outOfAmmo = new Event(i.actions());
			break;
			
			case EventID::ReloadEnd:
				reloadEnd = new Event(i.actions());
			break;
		}
	}
	
	return true;
}

bool WeaponType::load2(const fs::path &filename)
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
						currEvent = new Event();
						primaryShoot = currEvent;
					}
					else if ( eventName == "primary_press" )
					{
						currEvent = new Event();
						primaryPressed = currEvent;
					}
					else if ( eventName == "primary_release" )
					{
						currEvent = new Event();
						primaryReleased = currEvent;
					}
					else if ( eventName == "out_of_ammo" )
					{
						currEvent = new Event();
						outOfAmmo = currEvent;
					}
					else if ( eventName == "reaload_end" )
					{
						currEvent = new Event();
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

