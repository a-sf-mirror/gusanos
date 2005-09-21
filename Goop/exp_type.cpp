#include "exp_type.h"

#include "resource_list.h"

#include "events.h"
#ifndef DEDSERV
#include "sprite_set.h"
#include "distortion.h"
#include "gfx.h"
#endif //DEDSERV
#include "text.h"
#include "parser.h"
#include "detect_event.h"
#include "object_grid.h"



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

ExpType::ExpType()
: wupixels(0)
{
	timeout = 0;
	timeoutVariation = 0;

	colour = -1;
	alpha = 255;
	destAlpha = -1;	
	renderLayer = Grid::WormRenderLayer;
#ifndef DEDSERV
	sprite = NULL;
	distortion = NULL;
	distortMagnitude = 0.8;
	
	blender = BlitterContext::None;
#endif
	
	creation = NULL;
}

ExpType::~ExpType()
{
	delete creation;
#ifndef DEDSERV
	delete distortion;
#endif
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
#ifndef DEDSERV
					if ( var == "sprite" ) sprite = spriteList.load(val);
#else
					if ( var == "sprite" ) /* ignore */;
#endif
					else if ( var == "timeout" ) timeout = cast<int>(val);
					else if ( var == "timeout_variation" ) timeoutVariation = cast<int>(val);
					else if ( var == "render_layer" ) renderLayer = cast<int>(val);
					else if ( var == "alpha" ) alpha = cast<int>(val);
					else if ( var == "dest_alpha" ) destAlpha = cast<int>(val);
#ifndef DEDSERV
					else if ( var == "blender" )
					{
						if ( val == "add" ) blender = BlitterContext::Add;
						else if ( val == "alpha" ) blender = BlitterContext::Alpha;
					}
#else
					else if ( var == "blender" ) /* ignore */;
#endif
					else if ( var == "wu_pixels" ) 
						wupixels = ( cast<int>(val) != 0 );
					else if ( var == "colour" || var == "color" )
					{
						if ( tokens.size() >= 5 )
							colour = makecol( cast<int>(tokens[2]), cast<int>(tokens[3]), cast<int>(tokens[4]) );
					}
#ifndef DEDSERV
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
#else
					else if ( var == "distortion" ) /* ignore*/;
					else if ( var == "distort_magnitude" ) /* ignore*/;
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
					if ( eventName == "creation" )
					{
						currEvent = new Event;
						creation = currEvent;
					}
					else if ( eventName == "detect_range" )
					{
						float range = 0;
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
								detectFilter |= 1 << (cast<int>(*iter) + 1);
							}
							++iter;
						}
						if ( !detectFilter ) detectFilter = 1;
						detectRanges.push_back( new DetectEvent(range, detectOwner, detectFilter));
						currEvent = detectRanges.back()->m_event;
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


