#include "gfx.h"
#include "text.h"
#include "gconsole.h"

#include <allegro.h>

#include <string>
#include <list>

using namespace std;

Gfx gfx;

Gfx::Gfx()
{
	buffer = NULL;
}

Gfx::~Gfx()
{
	if ( buffer ) destroy_bitmap(buffer);
}

void Gfx::init()
{	
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0);

	if(set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
		set_display_switch_mode(SWITCH_BACKGROUND);
	
	buffer = create_bitmap(screen->w,screen->h);
}

void Gfx::shutDown()
{
	if ( buffer ) destroy_bitmap(buffer);
}

void Gfx::registerInConsole()
{
	console.registerCommand("SCREENSHOT", screenShot);
	console.registerIntVariable("VID_VSYNC", &m_vsync, 1);
	console.registerIntVariable("VID_CLEAR_BUFFER", &m_clearBuffer, 1);
}

string screenShot(const list<string> &args)
{
	int nameIndex = 0;
	string filename = "screenshots/ss" + cast<string>(nameIndex) + ".pcx";
	while( exists( filename.c_str() ) )
	{
		nameIndex += 1;
		filename = "screenshots/ss" + cast<string>(nameIndex) + ".pcx";
	}
	
	BITMAP * tmpbitmap = create_bitmap(screen->w,screen->h);
	blit(screen,tmpbitmap,0,0,0,0,320,240);
	save_bitmap( filename.c_str(),tmpbitmap,0);
	destroy_bitmap(tmpbitmap);
	
	return "SCREENSHOT SAVED AS: SS" + cast<string>(nameIndex) + ".pcx";
}

