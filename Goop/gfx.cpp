#include "gfx.h"
#include "text.h"
#include "gconsole.h"

#include <allegro.h>

#include <loadpng.h>

#include <string>
#include <algorithm>
#include <list>

using namespace std;

Gfx gfx;

string fullscreenCmd(const list<string> &args)
{
	return "";
}

void fullscreen( int oldValue )
{
	gfx.fullscreenChange();
}

void doubleRes( int oldValue )
{
	gfx.doubleResChange();
}

Gfx::Gfx()
{
	buffer = NULL;
}

Gfx::~Gfx()
{
}

void Gfx::init()
{	
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0);
	m_fullscreen = true;
	m_doubleRes = false;
	m_vwidth = 320;
	m_vheight = 240;
	//set_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0);
	
	if(set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
		set_display_switch_mode(SWITCH_BACKGROUND);

	loadpng_init();

	buffer = create_bitmap(320,240);
}

void Gfx::shutDown()
{
	if ( buffer ) destroy_bitmap(buffer);
}

void Gfx::registerInConsole()
{
	console.registerCommand("SCREENSHOT", screenShot);
	console.registerCommand("VID_TOGGLE_FULLSCREEN", fullscreenCmd);
	console.registerIntVariable("VID_FULLSCREEN", &m_fullscreen, 1, fullscreen);
	console.registerIntVariable("VID_DOUBLERES", &m_doubleRes, 0, doubleRes);
	console.registerIntVariable("VID_VSYNC", &m_vsync, 1);
	console.registerIntVariable("VID_CLEAR_BUFFER", &m_clearBuffer, 1);
	
	map<string, int> videoFilters;
	videoFilters["NOFILTER"] = NO_FILTER;
	videoFilters["SCANLINES"] = SCANLINES;
	//videoFilters["2XSAI"] = AA2XSAI; // To be included later.
		
	console.registerEnumVariable("VID_FILTER", &m_filter, NO_FILTER, videoFilters);
}

void Gfx::updateScreen()
{
	if ( m_vsync ) vsync();
	if ( !m_doubleRes )
	{
		blit(buffer,screen,0,0,0,0,m_vwidth,m_vheight);
	}else
	{
		switch ( (Filters)m_filter )
		{
			case NO_FILTER: 
				stretch_blit(buffer, m_doubleResBuffer, 0, 0, buffer->w, buffer->h, 0, 0, m_doubleResBuffer->w, m_doubleResBuffer->h);
			break;
			case SCANLINES: 
				stretch_blit(buffer, m_doubleResBuffer, 0, 0, buffer->w, buffer->h, 0, 0, m_doubleResBuffer->w, m_doubleResBuffer->h);
				drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
				set_trans_blender(0, 0, 0, 128);
				for ( int i = 0; i < m_doubleResBuffer->h; ++++i )
				{
					hline(m_doubleResBuffer, 0, i, m_doubleResBuffer->w, 0);
				}
				solid_mode();
			break;
		}
		blit(m_doubleResBuffer,screen,0,0,0,0,m_vwidth,m_vheight);
	}
	if ( m_clearBuffer ) clear_bitmap(buffer);
}

void Gfx::fullscreenChange()
{
	set_color_depth(16);
	
	if ( m_fullscreen )
	{
		set_gfx_mode(GFX_AUTODETECT, m_vwidth, m_vheight, 0, 0);
	}else
	{
		set_gfx_mode(GFX_AUTODETECT_WINDOWED, m_vwidth, m_vheight, 0, 0);
	}
	
	if(set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
		set_display_switch_mode(SWITCH_BACKGROUND);
}

void Gfx::doubleResChange()
{
	if ( m_doubleResBuffer ) destroy_bitmap( m_doubleResBuffer );
	if ( m_doubleRes )
	{
		m_vwidth = 640;
		m_vheight = 480;
		m_doubleResBuffer = create_bitmap(m_vwidth, m_vheight );
	}else
	{
		m_vwidth = 320;
		m_vheight = 240;
		m_doubleResBuffer = NULL;
	}
	fullscreenChange();
}

BITMAP* Gfx::loadBitmap( const string& filename, RGB* palette )
{
	BITMAP* returnValue = NULL;
	
	if ( exists( filename.c_str() ) )
	{
		string extension = filename.substr(filename.length() - 3);
		transform(extension.begin(), extension.end(), extension.begin(), (int(*)(int)) toupper);
		
		if ( extension == "PNG" )
		{
			returnValue = load_png(filename.c_str(), palette);
		}
		else
		{
			returnValue = load_bitmap(filename.c_str(), palette);
		}
	}else
	{
		string tmp = filename;
		tmp += ".png";
		if ( exists( tmp.c_str() ) )
		{
			returnValue = load_png( tmp.c_str() , palette );
		}
		else
		{
			tmp = filename;
			tmp += ".bmp";
			if ( exists( tmp.c_str() ))
			{
				returnValue = load_bmp( tmp.c_str() , palette );
			}
		}
	}
	
	return returnValue;
}

bool Gfx::saveBitmap( const string &filename,BITMAP* image, RGB* palette )
{
	bool returnValue = false;
	
	string extension = filename.substr(filename.length() - 3);
	transform(extension.begin(), extension.end(), extension.begin(), (int(*)(int)) toupper);
	
	if ( extension == "PNG" )
	{
		if ( !save_png(filename.c_str(), image, palette) ) returnValue = true;
	}
	else
	{
		if ( !save_bitmap(filename.c_str(), image, palette) ) returnValue = true;
	}
	
	return returnValue;
}

string screenShot(const list<string> &args)
{
	int nameIndex = 0;
	string filename = "screenshots/ss" + cast<string>(nameIndex) + ".png";
	while( exists( filename.c_str() ) )
	{
		nameIndex += 1;
		filename = "screenshots/ss" + cast<string>(nameIndex) + ".png";
	}
	
	BITMAP * tmpbitmap = create_bitmap(screen->w,screen->h);
	blit(screen,tmpbitmap,0,0,0,0,screen->w,screen->h);
	bool success = gfx.saveBitmap( filename.c_str(),tmpbitmap,0);
	destroy_bitmap(tmpbitmap);
	
	if ( success )
		return "SCREENSHOT SAVED AS: SS" + cast<string>(nameIndex) + ".png";
	else 
		return "UNABLE TO SAVE SCREENSHOT";
}

