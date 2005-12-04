#include "level_effect.h"

#include "resource_list.h"

#include "gfx.h"
#include "util/text.h"
#include "parser.h"
#include "sprite_set.h"
#include "omfg_script.h"
#include "game_actions.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
namespace fs = boost::filesystem;

using namespace std;

ResourceList<LevelEffect> levelEffectList;

LevelEffect::LevelEffect()
: ResourceBase()
, mask(NULL)
{
	
}

LevelEffect::~LevelEffect()
{
}

bool LevelEffect::load(fs::path const& filename)
{
	fs::ifstream fileStream(filename);

	if (!fileStream )
		return false;
	
	OmfgScript::Parser parser(fileStream, gameActions, filename.native_file_string());
	
	if(!parser.run())
	{
		if(parser.incomplete())
			parser.error("Trailing garbage");
		return false;
	}
	
	{
		OmfgScript::TokenBase* v = parser.getProperty("mask");
		if(!v->isDefault())
			mask = spriteList.load(v->toString());
	}

	return true;
}


