#ifndef MENU_WINDOW_H
#define MENU_WINDOW_H

#include <allegro.h>

class MenuWindow
{
	public:
	
	MenuWindow();
	~MenuWindow();
	
	void draw(BITMAP *where);
	void proccess();
	
	private:
	
	int m_width;
	int m_height;
	int m_x;
	int m_y;
	
	BITMAP * m_titleBar;
	BITMAP * m_panel;
	
};

#endif // _MENU_WINDOW_H_
