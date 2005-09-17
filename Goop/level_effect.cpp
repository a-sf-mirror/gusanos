#include "level_effect.h"

#include "resource_list.h"

#include "gfx.h"
#include "text.h"
#include "parser.h"
#include "sprite_set.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
namespace fs = boost::filesystem;

using namespace std;

ResourceList<LevelEffect> levelEffectList("mapeffects/");

LevelEffect::LevelEffect() : ResourceBase()
{
	mask = NULL;
}

LevelEffect::~LevelEffect()
{
}

bool LevelEffect::load(fs::path const& filename)
{

	fs::ifstream fileStream(filename);

	if ( fileStream )
	{
		string parseLine;
		while ( portable_getline( fileStream, parseLine ) )
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
								
				if ( var == "mask" ) mask = spriteList.load(val);

				else
				{
					std::cout << "Unknown variable on following line:" << std::endl;
					std::cout << "\t" << parseLine << std::endl;
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


