#ifndef DEDSERV

#include "font.h"

//#include "resource_list.h"
#include "resource_locator.h"
#include "blitters/blitters.h"

#include <allegro.h>
#include <string>
#include <vector>
#include <utility>
#include <iostream>

using namespace std;

array<Font::Color, 16> Font::palette =
{
	Color(255, 255, 255), //0
	Color(0, 0, 0),       //1
	Color(255, 0, 0),     //2
	Color(0, 255, 0),     //3
	Color(0, 0, 255),     //4
	Color(255, 255, 0),   //5
	Color(0, 255, 255),   //6
	Color(255, 0, 255),   //7
	Color(90, 90, 90),    //8
	Color(170, 170, 170), //9
	Color(128, 0, 0),     //10
	Color(0, 128, 0),     //11
	Color(0, 0, 128),     //12
	Color(128, 128, 0),   //13
	Color(0, 128, 128),   //14
	Color(128, 0, 128),   //15
};

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

void Font::draw( BITMAP* where, string::const_iterator b, string::const_iterator e, int x, int y, int spacing, int cr, int cg, int cb, int fact, int flags)
{
	int color = makecol(cr, cg, cb);
	for(; b != e; ++b)
	{
		CharInfo *c = lookupChar(*b);
		
		//masked_blit(m_bitmap, where, c->rect.x1, c->rect.y1, x, y, c->width, c->height);
		if(c->subBitmap)
		{
			if(flags & Shadow)
				drawSprite_blendtint(where, c->subBitmap, x + 2, y + 2, fact / 2, 0);
			drawSprite_blendtint(where, c->subBitmap, x, y, fact, color);
			
		}

		x += c->width + c->spacing + spacing;
	}
}

void Font::drawFormatted( BITMAP* where, string::const_iterator b, string::const_iterator e, int x, int y, int spacing, int cr, int cg, int cb, int fact, int flags)
{
	CharFormatting format(CharFormatting::Item(Color(cr, cg, cb)));
	for(; b != e; ++b)
	{
		while(checkFormatting(format, b, e))
		{
			if(b == e)
				return;
		}
		CharInfo *c = lookupChar(*b);
		
		if(c->subBitmap)
		{
			if(flags & Shadow)
				drawSprite_blendtint(where, c->subBitmap, x + 2, y + 2, fact / 2, 0);
			drawSprite_blendtint(where, c->subBitmap, x, y, fact, format.cur.color.toAllegro());
		}

		x += c->width + c->spacing + spacing;
	}
}

pair<int, int> Font::getDimensions(std::string const& text, int spacing)
{
	return getDimensions(text.begin(), text.end(), spacing);
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

pair<int, int> Font::getFormattedDimensions(std::string const& text, int spacing)
{
	return getFormattedDimensions(text.begin(), text.end(), spacing);
}

pair<int, int> Font::getFormattedDimensions(std::string::const_iterator b, std::string::const_iterator e, int spacing)
{
	if(b == e)
		return zeroDimensions();
	
	while(skipFormatting(b, e))
	{
		if(b == e)
			return zeroDimensions();
	}
	CharInfo *c = lookupChar(*b);
	int w = c->width;
	int h = c->height;
	
	++b;
	
	for(; b != e; ++b)
	{
		w += c->spacing + spacing;
		while(skipFormatting(b, e))
		{
			if(b == e)
				return make_pair(w, h);
		}
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