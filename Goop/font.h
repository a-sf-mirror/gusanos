#ifndef FONT_H
#define FONT_H

#ifdef DEDSERV
#error "Can't use this in dedicated server"
#endif //DEDSERV

//#include "resource_list.h"
#include "resource_locator.h"
#include <omfggui.h> // For Rect

#include <allegro.h>
#include <string>
#include <vector>
#include <utility>
#include <boost/array.hpp>
using boost::array;

class Font
{
public:
	enum Flags
	{
		Shadow = (1<<0),
	};
	
	struct Color
	{
		Color()
		{
		}
		
		Color(int r_, int g_, int b_)
		: r(r_), g(g_), b(b_)
		{
		}
		
		int toAllegro()
		{
			return makecol(r, g, b);
		}
		
		int r;
		int g;
		int b;
	};
	
	static array<Color, 16> palette;
	
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
	
	struct CharFormatting
	{
		struct Item
		{
			Item()
			{
			}
			
			Item(Color const& color_)
			: color(color_)
			{
			}
			
			Color color;
		};
		
		CharFormatting(Item item_)
		: cur(item_), loc(0)
		{
		}
		
		void push()
		{
			if((size_t)loc < stack.size())
				stack[loc] = cur;
			++loc;
		}
		
		void pop()
		{
			--loc;
			if((size_t)loc < stack.size())
				cur = stack[loc];
		}
		
		Item cur;
		int loc;
		array<Item, 5> stack;
	};
		
	Font();
	~Font();
	
	void free();

	void draw(BITMAP* where, std::string const& text, int x, int y, int spacing = 0, int cr = 255, int cg = 255, int cb = 255, int fact = 256, int flags = 0)
	{
		draw(where, text.begin(), text.end(), x, y, spacing, cr, cg, cb, fact, flags);
	}
	
	void draw(BITMAP* where, std::string::const_iterator b, std::string::const_iterator e, int x, int y, int spacing = 0, int cr = 255, int cg = 255, int cb = 255, int fact = 256, int flags = 0);
	
	void drawFormatted(BITMAP* where, std::string const& text, int x, int y, int spacing = 0, int cr = 255, int cg = 255, int cb = 255, int fact = 256, int flags = 0)
	{
		drawFormatted(where, text.begin(), text.end(), x, y, spacing, cr, cg, cb, fact, flags);
	}
	
	void drawFormatted(BITMAP* where, std::string::const_iterator b, std::string::const_iterator e, int x, int y, int spacing = 0, int cr = 255, int cg = 255, int cb = 255, int fact = 256, int flags = 0);
	
	CharInfo* lookupChar(char c);
	
	template<class IteratorT>
	char readChar(IteratorT& p, IteratorT end)
	{
		if(p != end)
			return *p++;
		return 0;
	}
		
	template<class IteratorT>
	bool checkFormatting(CharFormatting& format, IteratorT& p, IteratorT end)
	{
		char c = *p;
		switch(c)
		{
			case '\013':
			{
				++p;
				int colIndexA = readChar(p, end) - '0';
				int colIndexB = readChar(p, end) - '0';
				
				int colIndex = colIndexA * 10 + colIndexB;
				if((size_t)colIndex >= palette.size())
					colIndex = 0;
				
				format.cur.color = palette[colIndex];
				
				return true;
			}
			break;
			
			case '{':
			{
				++p;
				format.push();
				return true;
			}
			break;
			
			case '}':
			{
				++p;
				format.pop();
				return true;
			}
			break;
		}
		
		return false;
	}
	
	template<class IteratorT>
	bool skipFormatting(IteratorT& p, IteratorT end)
	{
		char c = *p;
		switch(c)
		{
			case '\013':
			{
				++p;
				readChar(p, end);
				readChar(p, end);
				return true;
			}
			break;
			
			case '{': case '}':
			{
				++p;
				return true;
			}
			break;
		}
		
		return false;
	}
	
	// Returns the dimensions of 'text' when printed with the spacing 'spacing'
	std::pair<int, int> getDimensions(std::string const& text, int spacing = 0);
	
	std::pair<int, int> getDimensions(std::string::const_iterator b, std::string::const_iterator e, int spacing = 0);
	
	std::pair<int, int> getFormattedDimensions(std::string const& text, int spacing = 0);
	
	std::pair<int, int> getFormattedDimensions(std::string::const_iterator b, std::string::const_iterator e, int spacing = 0);
	
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
