#include "liero.h"
#include "../gfx.h"
#include "zlib.h"
#include <string>
#include <cstring>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;

LieroLevelLoader LieroLevelLoader::instance;

bool LieroLevelLoader::canLoad(fs::path const& path, std::string& name)
{
	if(fs::extension(path) == ".lev")
	{
		name = basename(path);
		return true;
	}
	return false;
}

bool LieroLevelLoader::load(Level* level, fs::path const& path)
{
	fs::ifstream f(path);
	if(!f)
		return false;
		
	f.seekg(0, std::ios::end);
	std::streamoff fileSize = f.tellg();
	f.seekg(0, std::ios::beg);
	
	const std::streamoff regularFileSize = 504*350;
	if(fileSize < regularFileSize)
		return false;
		
	return false;
	
	/* TODO
	set_color_depth(8);
	level->material = create_bitmap_ex(8, width, height);
	set_color_depth(vdepth);
	level->image = create_bitmap_ex(vdepth, width, height);
	level->background = create_bitmap_ex(vdepth, width, height);

	unsigned char *pbackground = &data[0];
	unsigned char *pimage = pbackground + width*height*3;
	unsigned char *pmaterial = pimage + width*height*3;
	
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			
			int backgroundc = makecol(pbackground[0], pbackground[1], pbackground[2]);
			
			int m = pmaterial[0];
			++mo[m];
			
			if(m == 1)
				putpixel(level->image, x, y, backgroundc);
			else
			{
				int imagec = makecol(pimage[0], pimage[1], pimage[2]);
				putpixel(level->image, x, y, imagec);
			}
				
			putpixel(level->background, x, y, backgroundc);
			putpixel(level->material, x, y, m);
			
			pimage += 3;
			backgroundc += 3;
			++pmaterial;
		}
	}
	
	//TODO
	level->m_materialList[2].worm_pass = true;
	level->m_materialList[2].particle_pass = true;
	
	level->m_materialList[3].worm_pass = false;
	level->m_materialList[3].particle_pass = false;
	
	level->m_materialList[5].worm_pass = false;
	level->m_materialList[5].particle_pass = false;
	
	level->loaded = true;
	return true;
	*/
}

const char* LieroLevelLoader::getName()
{
	return "LieroX level loader";
}
