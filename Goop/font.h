#ifndef FONT_H
#define FONT_H

#include "resource_list.h"

#include <allegro.h>
#include <string>
#include <vector>

class Font
{
	public:
		
	Font();
	~Font();
	
	bool load(const std::string &filename);
	void draw(BITMAP *where, const std::string &text ,int x, int y , int space);
	int height();
	int width();
	
	private:
	
	std::vector<BITMAP*> m_char;

};

extern ResourceList<Font> fontList;

#endif // _FONT_H_
