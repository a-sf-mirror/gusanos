#include "font.h"

//#include "resource_list.h"
#include "resource_locator.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <utility>

using namespace std;

//ResourceList<Font> fontList("fonts/");
ResourceLocator<Font> fontLocator;

Font::Font()
: m_bitmap(0)
{
	
}

Font::~Font()
{
	/*
	vector<BITMAP*>::iterator iter = m_char.begin();
	while (iter != m_char.end())
	{
		destroy_bitmap(*iter);
		++iter;
	}*/
	
	free();
}

void Font::free()
{
	m_chars.clear();
	if(m_bitmap)
		destroy_bitmap(m_bitmap);
	m_bitmap = 0;
}

bool Font::load(string const& filename)
{

	/*
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
	*/
	return false;
}

Font::CharInfo* Font::lookupChar(char c)
{
	unsigned int idx = (unsigned int)c;
	if(idx >= m_chars.size())
		return &m_chars[0];
		
	return &m_chars[idx];
}

void Font::draw( BITMAP* where, string::const_iterator b, string::const_iterator e, int x, int y, int spacing)
{
	for(; b != e; ++b)
	{
		CharInfo *c = lookupChar(*b);
		
		masked_blit(m_bitmap, where, c->rect.x1, c->rect.y1, x, y, c->width, c->height);
		
		x += c->width + c->spacing + spacing;
	}
}

pair<int, int> Font::getDimensions(std::string const& text, int spacing)
{
	if(text.empty())
		return zeroDimensions();
		
	string::const_iterator i = text.begin();
	CharInfo *c = lookupChar(*i);
	int w = c->width;
	int h = c->height;
	
	++i;
	
	for(; i != text.end(); ++i)
	{
		w += c->spacing + spacing;
		c = lookupChar(*i);
		w += c->width;
		if(c->height > h)
			h = c->height;
	}
	
	return make_pair(w, h);
}



pair<int, int> Font::zeroDimensions()
{
	return make_pair(0, 0);
}

void Font::incrementDimensions(pair<int, int>& dim, char ch, int spacing)
{
	CharInfo *c = lookupChar(ch);
	dim.first += c->width + c->spacing + spacing;

	if(c->height > dim.second)
		dim.second = c->height;
}

void Font::removeSpacing(pair<int, int>& dim, char ch, int spacing)
{
	CharInfo *c = lookupChar(ch);
	dim.first -= c->spacing + spacing;
}

/*
int Font::width()
{
	return m_char[0]->w;
}

int Font::height()
{
	return m_char[0]->h;
}

*/
