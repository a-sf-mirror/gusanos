#include "font.h"

#include "resource_list.h"

#include <allegro.h>
#include <string>
#include <vector>

using namespace std;

ResourceList<Font> fontList("fonts/");

Font::Font()
{
	
}

Font::~Font()
{
	vector<BITMAP*>::iterator iter = m_char.begin();
	while (iter != m_char.end())
	{
		delete *iter;
		++iter;
	}
}

bool Font::load(const string &filename)
{
	BITMAP *tempBitmap = load_bmp(filename.c_str(),0);
	if (tempBitmap)
	{
		int width = tempBitmap->w / 256;
		for (int i = 0; i < 256; ++i)
		{
			BITMAP* character = create_bitmap(width,tempBitmap->h);
			blit(tempBitmap,character,i * width,0,0,0,width,character->h);
			m_char.push_back(character);
		}
		destroy_bitmap(tempBitmap);
		return true;
	}
	return false;
}

void Font::draw( BITMAP* where, const string &text, int x, int y, int space)
{
	if ( !text.empty() )
	{
		int width = m_char[0]->w;
		char character;
		for (int i = 0; i < text.length(); ++i)
		{
			character = text[i];
			draw_sprite(where, m_char[character], x + i * (width + space), y);
		}
	}
}

int Font::width()
{
	return m_char[0]->w;
}

int Font::height()
{
	return m_char[0]->h;
}

