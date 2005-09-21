#ifndef DEDSERV

#include "font.h"

//#include "resource_list.h"
#include "resource_locator.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <utility>
#include <iostream>

using namespace std;

//ResourceList<Font> fontList("fonts/");
ResourceLocator<Font> fontLocator;

Font::Font()
: m_bitmap(0), m_supportColoring(false)
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
	for(std::vector<CharInfo>::iterator i = m_chars.begin(); i != m_chars.end(); ++i)
	{
		destroy_bitmap(i->subBitmap);
	}
	
	m_chars.clear();

	destroy_bitmap(m_bitmap);
	
	m_bitmap = 0;
}

Font::CharInfo* Font::lookupChar(char c)
{
	unsigned int idx = (unsigned int)c;
	if(idx >= m_chars.size())
		return &m_chars[0];
		
	return &m_chars[idx];
}

void Font::draw( BITMAP* where, string::const_iterator b, string::const_iterator e, int x, int y, int spacing, int cr, int cg, int cb)
{/*
	if(!m_supportColoring)
	{
		for(; b != e; ++b)
		{
			CharInfo *c = lookupChar(*b);
			
			masked_blit(m_bitmap, where, c->rect.x1, c->rect.y1, x, y, c->width, c->height);
			//draw_character_ex(where, c->subBitmap, x, y, color, -1);
			
			
			x += c->width + c->spacing + spacing;
		}
	}
	else
	{*/
	int color = makecol(cr, cg, cb);
	for(; b != e; ++b)
	{
		CharInfo *c = lookupChar(*b);
		
		//masked_blit(m_bitmap, where, c->rect.x1, c->rect.y1, x, y, c->width, c->height);
		if(c->subBitmap)
			draw_character_ex(where, c->subBitmap, x, y, color, -1);

		x += c->width + c->spacing + spacing;
	}
	/*}*/
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

pair<int, int> Font::getDimensions(std::string::const_iterator b, std::string::const_iterator e, int spacing)
{
	if(b == e)
		return zeroDimensions();
		
	CharInfo *c = lookupChar(*b);
	int w = c->width;
	int h = c->height;
	
	++b;
	
	for(; b != e; ++b)
	{
		w += c->spacing + spacing;
		c = lookupChar(*b);
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

void Font::buildSubBitmaps()
{
	for(std::vector<CharInfo>::iterator i = m_chars.begin(); i != m_chars.end(); ++i)
	{
		i->subBitmap = create_sub_bitmap(m_bitmap, i->rect.x1, i->rect.y1, i->width, i->height);
	}
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

#endif //DEDSERV