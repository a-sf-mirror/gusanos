#include "gusanos.h"
#include "../gfx.h"
//#include "../game.h"
#include "../glua.h"
#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;

#include <iostream>
using namespace std;

GusanosLevelLoader GusanosLevelLoader::instance;

bool GusanosLevelLoader::canLoad(fs::path const& path, std::string& name)
{
	if(fs::exists(path / "config.cfg"))
	{
		name = path.leaf();
		return true;
	}
	return false;
}
	
bool GusanosLevelLoader::load(Level* level, fs::path const& path)
{
	int vdepth = get_color_depth();
	
	std::string materialPath = (path / "material").native_file_string();
	
	level->path = path.native_directory_string();
	
	set_color_depth(8);
	level->material = gfx.loadBitmap(materialPath.c_str(), 0);
	set_color_depth(vdepth);
	if (level->material)
	{
		std::string imagePath = (path / "level").native_file_string();
		
		level->image = gfx.loadBitmap(imagePath.c_str(), 0);
		if (level->image)
		{
			std::string backgroundPath = (path / "background").native_file_string();
			
			level->background = gfx.loadBitmap(backgroundPath.c_str(),0);
			if(!level->background)
			{
				level->background = create_bitmap(level->material->w, level->material->h);
				blit(level->image, level->background, 0,0,0,0,level->material->w, level->material->h);
				gfx.setBlender(ALPHA,120);
				rectfill( level->background, 0,0,level->background->w,level->background->h,0);
				solid_mode();
			}
			
			level->loaderSucceeded();
			return true;
		}
	}
	level->unload();
	return false;
}

const char* GusanosLevelLoader::getName()
{
	return "Gusanos 0.9 level loader";
}

GusanosFontLoader GusanosFontLoader::instance;

bool GusanosFontLoader::canLoad(fs::path const& path, std::string& name)
{
	if(fs::extension(path) == ".bmp")
	{
		name = basename(path);
		return true;
	}
	return false;
}
	
bool GusanosFontLoader::load(Font* font, fs::path const& path)
{
	font->free();
	
	int oldColorDepth = get_color_depth();
	int oldColorConversion = get_color_conversion();
	set_color_depth(8);
	set_color_conversion(COLORCONV_REDUCE_TO_256 | COLORCONV_KEEP_TRANS);

	font->m_bitmap = load_bmp(path.native_file_string().c_str(), 0);
	if(!font->m_bitmap)
		return false;
		
	set_color_conversion(oldColorConversion);
	set_color_depth(oldColorDepth);
		
	font->m_supportColoring = true;
		
	int monoWidth = font->m_bitmap->w / 256;
	int monoHeight = font->m_bitmap->h;
	
	if(monoWidth <= 0 || monoHeight <= 0)
		return false;

	int x = 0;
	for (int i = 0; i < 256; ++i)
	{
		/*BITMAP* character = create_bitmap(width,tempBitmap->h);
		blit(tempBitmap,character,i * width,0,0,0,width,character->h);*/
		font->m_chars.push_back(Font::CharInfo(Rect(x, 0, x + monoWidth, monoHeight), 0));
		
		x += monoWidth;
	}
	
	font->buildSubBitmaps();
	
	return true;
	/*
	BITMAP *tempBitmap = load_bmp(path.native_file_string().c_str(),0);
	if (tempBitmap)
	{
		int width = tempBitmap->w / 256;
		for (int i = 0; i < 256; ++i)
		{
			BITMAP* character = create_bitmap(width,tempBitmap->h);
			blit(tempBitmap,character,i * width,0,0,0,width,character->h);
			font->m_char.push_back(character);
		}
		destroy_bitmap(tempBitmap);
		return true;
	}

	return false;*/
}

const char* GusanosFontLoader::getName()
{
	return "Gusanos 0.9 font loader";
}

XMLLoader XMLLoader::instance;

bool XMLLoader::canLoad(fs::path const& path, std::string& name)
{
	if(fs::extension(path) == ".xml")
	{
		name = basename(path);
		return true;
	}
	return false;
}
	
bool XMLLoader::load(XMLFile* xml, fs::path const& path)
{
	xml->f.open(path, std::ios::binary);
	
	if(!xml->f)
		return false;
		
	return true;
}

const char* XMLLoader::getName()
{
	return "XML loader";
}


GSSLoader GSSLoader::instance;

bool GSSLoader::canLoad(fs::path const& path, std::string& name)
{
	if(fs::extension(path) == ".gss")
	{
		name = basename(path);
		return true;
	}
	return false;
}
	
bool GSSLoader::load(GSSFile* gss, fs::path const& path)
{
	gss->f.open(path, std::ios::binary);
	
	if(!gss->f)
		return false;
		
	return true;
}

const char* GSSLoader::getName()
{
	return "GSS loader";
}


LuaLoader LuaLoader::instance;

bool LuaLoader::canLoad(fs::path const& path, std::string& name)
{
	if(fs::extension(path) == ".lua")
	{
		name = basename(path);
		return true;
	}
	return false;
}
	
bool LuaLoader::load(Script* script, fs::path const& path)
{
	fs::ifstream f(path, std::ios::binary);
	if(!f)
		return false;
		
	// Create the table to store the functions in	
	std::string name = basename(path);
	lua_pushstring(lua, name.c_str());
	lua_rawget(lua, LUA_GLOBALSINDEX);
	if(lua_isnil(lua, -1))
	{
		// The table does not exist, create it
		
		lua_pushstring(lua, name.c_str());
		lua_newtable(lua);
		lua_rawset(lua, LUA_GLOBALSINDEX);
	}
	lua_settop(lua, -2); // Pop table or nil
	
	lua.load(f);
	
	script->lua = &lua;
	script->table = name;
	
	return true;
}

const char* LuaLoader::getName()
{
	return "Lua loader";
}
