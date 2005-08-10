#ifndef FONT_H
#define FONT_H

//#include "resource_list.h"
#include "resource_locator.h"
#include <omfggui.h> // For Rect

#include <allegro.h>
#include <string>
#include <vector>
#include <utility>

class Font
{
public:
	struct CharInfo
	{
		CharInfo()
		: subBitmap(0)
		{
		}
		
		CharInfo(Rect const& rect_, int spacing_)
		: rect(rect_), spacing(spacing_), subBitmap(0)
		{
			width = rect.getWidth();
			height = rect.getHeight();
		}
		
		~CharInfo()
		{
			// Don't delete anything here!
		}
		
		Rect rect;
		int  width; // The width of the character (= rect.getWidth())
		int  height; // The height of the character (= rect.getHeight())
		int  spacing; // Extra spacing to the next character
		BITMAP* subBitmap;
	};
		
	Font();
	~Font();
	
	void free();

	void draw(BITMAP* where, std::string const& text, int x, int y, int spacing = 0, int cr = 255, int cg = 255, int cb = 255)
	{
		draw(where, text.begin(), text.end(), x, y, spacing, cr, cg, cb);
	}
	
	void draw(BITMAP* where, std::string::const_iterator b, std::string::const_iterator e, int x, int y, int spacing = 0, int cr = 255, int cg = 255, int cb = 255);
	
	CharInfo* lookupChar(char c);
	
	// Returns the dimensions of 'text' when printed with the spacing 'spacing'
	std::pair<int, int> getDimensions(std::string const& text, int spacing = 0);
	
	std::pair<int, int> getDimensions(std::string::const_iterator b, std::string::const_iterator e, int spacing = 0);
	
	// Returns an iterator to the first character between b and e that doesn't
	// fit in 'space'
	template<class IteratorT>
	IteratorT fitString(IteratorT b, IteratorT e, int space, int spacing = 0)
	{
		for(; b != e; ++b)
		{
			CharInfo* c = lookupChar(*b);
			if(c->width > space)
				return b;
			space -= c->width - c->spacing - spacing;
		}
		
		return e;
	}
	
	// Dimension calculating version
	template<class IteratorT>
	IteratorT fitString(IteratorT b, IteratorT e, int space, std::pair<int, int>& dim, int spacing = 0)
	{
		dim.second = 0;
		
		int oldSpace = space;
		
		for(; b != e; ++b)
		{
			CharInfo* c = lookupChar(*b);
			
			if(c->width > space)
			{
				dim.first = oldSpace - space; // TODO: Remove spacing for last character
				return b;
			}

			space -= c->width - c->spacing - spacing;

			if(c->height > dim.second)
				dim.second = c->height;
		}
		
		dim.first = oldSpace - space; // TODO: Remove spacing for last character
		return e;
	}
	
	// Returns a (0, 0) pair
	std::pair<int, int> zeroDimensions();
	
	// Adds a character 'ch' to the dimensions 'dim' printed with spacing 'spacing'
	// UNTESTED
	void incrementDimensions(std::pair<int, int>& dim, char ch, int spacing);
	
	// Remove the spacing of character 'ch' and 'spacing' from 'dim'
	// (Usually done to remove the spacing of the last character in a string
	// passed incrementally using incrementDimensions)
	void removeSpacing(std::pair<int, int>& dim, char ch, int spacing);

	void buildSubBitmaps();
	std::vector<CharInfo> m_chars;
	BITMAP*               m_bitmap;
	bool                  m_supportColoring;

};

//extern ResourceList<Font> fontList; //deprecated
extern ResourceLocator<Font> fontLocator;

#endif // _FONT_H_
