#include "losp.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
namespace fs = boost::filesystem;

LOSPFontLoader LOSPFontLoader::instance;

bool LOSPFontLoader::canLoad(fs::path const& path, std::string& name)
{
	if(fs::extension(path) == ".lfn")
	{
		name = basename(path);
		return true;
	}
	return false;
}

/*
Font format:
	INT32 surfaceWidth
	INT32 surfaceHeight
	
	STRUCT Dimensions
		INT32 x
		INT32 y
		INT32 width
		INT32 height
	END
	
	Dimensions chars[224]
	UINT8      surface[surfaceWidth * surfaceHeight]
*/
	
bool LOSPFontLoader::load(Font* font, fs::path const& path)
{
	font->free();

	fs::ifstream f(path, std::ios::binary);
	if(!f)
		return false;
		
	long bitmapWidth = 0, bitmapHeight = 0;
	f.read((char *)&bitmapWidth, 4);
	f.read((char *)&bitmapHeight, 4);
		
	if(!f)
		return false;

	font->m_bitmap = create_bitmap(bitmapWidth, bitmapHeight);
	if(!font->m_bitmap)
		return false;
		
	font->m_chars.assign(32, Font::CharInfo(Rect(0, 0, 1, 1), 0));
	
	for(int i = 0; i < 224; ++i)
	{
		int x, y, w, h;
		f.read((char *)&x, 4);
		f.read((char *)&y, 4);
		f.read((char *)&w, 4);
		f.read((char *)&h, 4);
		
		if(!f)
			return false;

		font->m_chars.push_back(Font::CharInfo(Rect(x, y, x + w, y + h), 0));
	}
	
	for(int y = 0; y < bitmapWidth; ++y)
	{
		for(int x = 0; x < bitmapHeight; ++x)
		{
			char v;
			if(!f.get(v))
				return false;
				
			int c = v ? makecol(v, v, v) : makecol(255, 0, 255);

			putpixel(font->m_bitmap, x, y, c);
		}
	}
	return true;
}

const char* LOSPFontLoader::getName()
{
	return "LOSP font loader";
}
