#include "gusanos.h"
#include "../gfx.h"
#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;

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
	set_color_depth(8);
	std::string materialPath = (path / "material").native_file_string();
	
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
				for (int x = 0; x < level->background->w; x++)
				for (int y = 0; y < level->background->h; y++)
				{
					putpixel(level->background, x, y, getpixel(level->image, x, y));
				}
			}
			
			//TODO
			level->m_materialList[0].worm_pass = false;
			level->m_materialList[0].particle_pass = false;
			
			level->m_materialList[1].worm_pass = false;
			level->m_materialList[1].particle_pass = false;
			
			level->loaded = true;
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

	font->m_bitmap = load_bmp(path.native_file_string().c_str(), 0);
	if(!font->m_bitmap)
		return false;
		
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
