#include "menu_window.h"

#include <allegro.h>

using namespace std;

MenuWindow::MenuWindow()
{
	m_width = 100;
	m_height = 50;
	m_x = 10;
	m_y = 10;
	
	m_titleBar = create_bitmap( m_width, 7);
	clear_to_color( m_titleBar, makecol(255,64,64));
	
	rect(m_titleBar, 0, 0, m_titleBar->w-1, m_titleBar->h, makecol(255,255,255));
	
	m_panel = create_bitmap (m_width, m_height);
	clear_bitmap( m_panel );
	
	rect(m_panel, 0, -1, m_panel->w-1, m_panel->h-1, makecol(255,255,255));
	
}

MenuWindow::~MenuWindow()
{
	delete m_titleBar;
	delete m_panel;
}

void MenuWindow::draw( BITMAP* where)
{
	draw_sprite(where, m_titleBar, m_x, m_y);
	draw_sprite(where, m_panel, m_x, m_y + m_titleBar->h);
}

