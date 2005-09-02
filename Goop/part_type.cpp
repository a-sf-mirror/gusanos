#include "part_type.h"

#include "resource_list.h"

#include "sprite_set.h"
#include "gfx.h"
#include "distortion.h"
#include "text.h"
#include "parser.h"
#include "detect_event.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
namespace fs = boost::filesystem;

using namespace std;

ResourceList<PartType> partTypeList("objects/");

TimerEvent::TimerEvent(int _delay, int _delayVariation, int _triggerTimes)
{
	delay=_delay;
	delayVariation = _delayVariation;
	triggerTimes = _triggerTimes;
	event = new Event;
}

TimerEvent::~TimerEvent()
{
	delete event;
}

PartType::PartType()
{
	gravity = 0;
	bounceFactor = 1;
	groundFriction = 1;
	colour = -1;
	repeat = 1;
	alpha = 255;
	angularFriction = 0;
	animDuration = 100;
	animType = ANIM_LOOPRIGHT;
	animOnGround = 1;
	damping = 1;
	acceleration = 0;
	maxSpeed = -1;
	colLayer = 0;
	health = 100;
	
	renderLayer = 1;
	sprite = NULL;
	distortion = NULL;
	distortMagnitude = 0.8;
	
	blender = NONE;
	
	groundCollision = NULL;
	creation = NULL;
	death = NULL;
	
	for ( int i = 0; i < 16; ++i )
	{
		customEvents.push_back(NULL);
	}
}

PartType::~PartType()
{
	if (groundCollision) delete groundCollision;
	if (creation) delete creation;
	if (distortion) delete distortion;
	for ( vector<TimerEvent*>::iterator i = timer.begin(); i != timer.end(); i++)
	{
		delete *i;
	}
	for ( vector<DetectEvent*>::iterator i = detectRanges.begin(); i != detectRanges.end(); i++)
	{
		delete *i;
	}
}

bool PartType::load(fs::path const& filename)
{
	//cerr << "Loading particle: " << filename.native_file_string() << endl;
	
	fs::ifstream fileStream(filename);
	
	if ( fileStream )
	{
		string parseLine;
		Event *currEvent = NULL;
		while ( portable_getline( fileStream, parseLine ) )
		{
			//if ( !parseLine.empty() ) //Unnecessary
			{
				string var;
				string val;

/* //Unnecessary
#ifndef WINDOWS
				//Check for windows formatting on files
				if (parseLine[parseLine.length()-1] == '\r')
				{
					parseLine.erase(parseLine.length()-1);
				}
#endif*/

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
					
					if ( var == "gravity" ) gravity = cast<float>(val);
					else if ( var == "repeat" ) repeat = cast<int>(val);	
					else if ( var == "bounce_factor" ) bounceFactor = cast<float>(val);
					else if ( var == "ground_friction" ) groundFriction = cast<float>(val);
					else if ( var == "damping" ) damping = cast<float>(val);
					else if ( var == "acceleration" ) acceleration = cast<float>(val);
					else if ( var == "max_speed" ) maxSpeed = cast<float>(val);
					else if ( var == "angular_friction" ) angularFriction = cast<float>(val);
					else if ( var == "health" ) health = cast<float>(val);
					else if ( var == "col_layer" ) colLayer = cast<int>(val);
					else if ( var == "sprite" ) sprite = spriteList.load(val);
					else if ( var == "anim_duration" ) animDuration = cast<int>(val);
					else if ( var == "anim_on_ground" ) animOnGround = cast<int>(val);
					else if ( var == "anim_type" )
					{
						if ( val == "ping_pong" ) animType = ANIM_PINGPONG;
						else if ( val == "loop_right" ) animType = ANIM_LOOPRIGHT;
						else if ( val == "right_once" ) animType = ANIM_RIGHTONCE;
					}
					else if ( var == "render_layer" ) renderLayer = cast<int>(val);
					else if ( var == "alpha" ) alpha = cast<int>(val);
					else if ( var == "blender" )
					{
						if ( val == "add" ) blender = ADD;
						else if ( val == "alpha" ) blender = ALPHA;
					}
					else if ( var == "colour" || var == "color" )
					{
						if ( tokens.size() >= 5 )
						colour = makecol( cast<int>(tokens[2]), cast<int>(tokens[3]), cast<int>(tokens[4]) );
					}
					else if ( var == "distortion" && !distortion )
					{
						if ( val == "lens" && tokens.size() >= 4)
							distortion = new Distortion( lensMap( cast<int>(tokens[3]) ) );
						else if ( val == "swirl" && tokens.size() >= 4)
							distortion = new Distortion( swirlMap( cast<int>(tokens[3]) ) );
						else if ( val == "ripple" && tokens.size() >= 4)
							distortion = new Distortion( rippleMap( cast<int>(tokens[3]) ) );
						else if ( val == "random" && tokens.size() >= 4)
							distortion = new Distortion( randomMap( cast<int>(tokens[3]) ) );
						else if ( val == "spin" && tokens.size() >= 4)
							distortion = new Distortion( spinMap( cast<int>(tokens[3]) ) );
						else if ( val == "bitmap" && tokens.size() >= 4)
							distortion = new Distortion( bitmapMap( tokens[3] ) );
					}
					else if ( var == "distort_magnitude" ) distortMagnitude = cast<float>(val); //<GLIP> Fixed a spelling error, magnitud -> magnitude
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
					if ( eventName == "ground_collision" )
					{
						currEvent = new Event;
						groundCollision = currEvent;
					}
					else if ( eventName == "creation" )
					{
						currEvent = new Event;
						creation = currEvent;
					}
					else if ( eventName == "death" )
					{
						currEvent = new Event;
						death = currEvent;
					}
					else if ( eventName == "timer" )
					{
						int delay = 100;
						int delayVariation = 0;
						int triggerTimes = 0;
						iter++;
						if( iter != tokens.end())
						{
							delay = cast<int>(*iter);
							++iter;
						}
						if( iter != tokens.end())
						{
							delayVariation = cast<int>(*iter);
							++iter;
						}
						if( iter != tokens.end())
						{
							triggerTimes = cast<int>(*iter);
						}
						timer.push_back(new TimerEvent(delay, delayVariation, triggerTimes));
						currEvent = timer.back()->event;
						
					}
					else if ( eventName == "detect_range" )
					{
						float range = 10;
						bool detectOwner = true;
						int detectFilter = 0;
						iter++;
						if( iter != tokens.end())
						{
							range = cast<float>(*iter);
							++iter;
						}
						if( iter != tokens.end())
						{
							detectOwner = (bool)cast<int>(*iter);
							++iter;
						}
						while ( iter != tokens.end() )
						{
							if ( *iter == "worms" ) detectFilter |= 1;
							else
							{
								detectFilter |= 1 << ( cast<int>(*iter)+1 );
							}
							++iter;
						}
						if ( !detectFilter ) detectFilter = 1;
						detectRanges.push_back( new DetectEvent(range, detectOwner, detectFilter));
						currEvent = detectRanges.back()->m_event;
					}
					else if ( eventName == "custom_event" && iter!= tokens.end() )
					{
						iter++;
						size_t eventIndex = cast<size_t>(*iter);
						if ( eventIndex < customEvents.size() )
						{
							currEvent = new Event;
							customEvents[eventIndex] = currEvent;
						}
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
		//fileStream.close(); //RAII ffs
		return true;
	} else
	{
		return false;
	}
}


