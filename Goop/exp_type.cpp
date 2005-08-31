#include "exp_type.h"

#include "resource_list.h"

#include "events.h"
#include "distortion.h"
#include "sprite_set.h"
#include "gfx.h"
#include "distortion.h"
#include "text.h"
#include "parser.h"



#include <allegro.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
namespace fs = boost::filesystem;

using namespace std;

ResourceList<ExpType> expTypeList("objects/");

DetectEvent::DetectEvent( float range, bool detectOwner, int detectFilter)
	: m_detectFilter(detectFilter), m_range(range), m_detectOwner(detectOwner)
{
	event = new Event;
}

DetectEvent::~DetectEvent()
{
	delete event;
}

ExpType::ExpType()
{
	timeout = 0;
	timeoutVariation = 0;
	
	colour = -1;
	alpha = 255;
	destAlpha = -1;	
	renderLayer = 1;
	sprite = NULL;
	distortion = NULL;
	distortMagnitude = 0.8;
	
	blender = NONE;
	
	creation = NULL;
}

ExpType::~ExpType()
{
	if (creation) delete creation;
	if (distortion) delete distortion;
	for ( vector<DetectEvent*>::iterator i = detectRanges.begin(); i != detectRanges.end(); i++)
	{
		delete *i;
	}
}

bool ExpType::load(fs::path const& filename)
{
	//cerr << "Loading explosion: " << filename.native_file_string() << endl;
	fs::ifstream fileStream(filename);

	if ( fileStream )
	{
		string parseLine;
		Event *currEvent = NULL;
		while ( portable_getline( fileStream, parseLine ) )
		{
			//if ( !parseLine.empty() ) //portable_getline never returns empty lines
			{
				string var;
				string val;
		
/* Unnecessary with portable_getline	
#ifndef WINDOWS
				//Check for windows formatting on files
				if (parseLine[parseLine.length()-1] == '\r')
				{
					parseLine.erase(parseLine.length()-1);
				}
#endif
*/
			
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
									
					if ( var == "sprite" ) sprite = spriteList.load(val);
					else if ( var == "timeout" ) timeout = cast<int>(val);
					else if ( var == "timeout_variation" ) timeoutVariation = cast<int>(val);
					else if ( var == "render_layer" ) renderLayer = cast<int>(val);
					else if ( var == "alpha" ) alpha = cast<int>(val);
					else if ( var == "dest_alpha" ) destAlpha = cast<int>(val);
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
					{	// TODO: Factorize this
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
					else if ( var == "distort_magnitude" ) distortMagnitude = cast<float>(val);
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
					if ( eventName == "creation" )
					{
						currEvent = new Event;
						creation = currEvent;
					}
					else if ( eventName == "detect_range" )
					{
						float range = 0;
						bool detectOwner = true;
						int detectFilter = 1;
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
							detectFilter = 0;
							if ( *iter == "worms" ) detectFilter |= 1;
							else
							{
								detectFilter |= (int)pow( 2.f, cast<int>(*iter) );
							}
							++iter;
						}
						detectRanges.push_back( new DetectEvent(range, detectOwner, detectFilter));
						currEvent = detectRanges.back()->event;
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
		//fileStream.close(); // Use RAII ffs >:o
		return true;
	} else
	{
		return false;
	}
}


