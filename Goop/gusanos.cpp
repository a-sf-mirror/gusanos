#include <allegro.h>

#include "gconsole.h"
#include "resource_list.h"
#include "sprite.h"
#include "font.h"
#include "vec.h"
#include "level.h"
#include "game.h"
#include "viewport.h"
#include "part_type.h"
#include "particle.h"
#include "worm.h"
#include "player.h"
#include "text.h"
#include "gfx.h"
#include "sfx.h"
#include "distortion.h"
#include "player_ai.h"
#include "gui.h"
#include "guilist.h"
#include "network.h"
#include "keyboard.h"

#include "script.h"
#include "glua.h"


#if 1
#include "menu.h"
#endif

#ifdef WINDOWS
	#include <winalleg.h>
#endif

#include <string>
#include <vector>

#ifdef POSIX
#include <unistd.h>
#endif

using namespace std;

bool quit = false;
int showFps;
int showDebug;

//millisecond timer
volatile unsigned int _timer = 0;
void _timerUpdate(void) { _timer++; } END_OF_FUNCTION(_timerUpdate);

string Exit(const list<string> &args)
{
	quit = true;
	return "";
}

void startGame(const std::string&)
{
    //menu = false;
}

int main(int argc, char **argv)
{
	game.init(argc, argv);
	
#ifdef POSIX
	
#endif

/*
	gui::init();
	
	//Font *tempFont = fontList.load("minifont.bmp");
	BITMAP *gusLogo = gfx.loadBitmap("default/gui/gusanos.bmp");
	gui::windowDecor = gfx.loadBitmap("default/gui/window.bmp");
	gui::Container top;
	gui::Window window;
	gui::ListContainer glist;
	gui::ListDragItem gdrag;
	gui::ListButtonItem gstart;
	window.setTitle("MAIN MENU");
	window.setPosition(31, 31);
	window.setSize(96, 64);
	
	gstart.setText("NEW GAME");
	gstart.setEvent(startGame);
	
	glist.setPosition(31, 31);
	glist.setSize(96, 64);
	glist.setBackColor(0);
	glist.setSelColor(makecol(63, 63, 63));
	glist.addItem(&gstart);
	glist.addItem(&gdrag);
	
	window.addWidget(&glist);
	top.addWidget(&window);
*/

	//OmfgGUI::menu.testParseXML();

	console.registerVariables()
		("CL_SHOWFPS", &showFps, 1)
		("CL_SHOWDEBUG", &showDebug, 1)
	;
	
	console.registerCommands()
		(string("QUIT"), Exit)
	;
	

	//console.parseLine("BIND F12 SCREENSHOT; BIND ESC QUIT");
	console.parseLine("BIND F12 SCREENSHOT");
	console.parseLine("SETCHAR STOP \".\"");
	
	///* <GLIP> Stuff for me ;o
	//console.parseLine("BIND LEFT +P0_LEFT; BIND RIGHT +P0_RIGHT; BIND 2_PAD +P0_JUMP; BIND UP +P0_UP; BIND DOWN +P0_DOWN; BIND 1_PAD +P0_FIRE; BIND 3_PAD +P0_CHANGE");
	console.parseLine("SETSHIFTCHAR MINUS \"_\" ; SETSHIFTCHAR 2 \"\\\"\" ; SETSHIFTCHAR COMMA \";\" ; SETALTGRCHAR 7 \"\\{\" ; SETALTGRCHAR 0 \"\\}\"");
	console.parseLine("SETALTGRCHAR PLUS_PAD \"\\\\\" ; SETSHIFTCHAR 3 \"#\" ; SETSHIFTCHAR STOP \":\"");
	//*/
	
	OmfgGUI::menu.clear();
	game.loadMod();
	
	//install millisecond timer
	install_timer();
	LOCK_VARIABLE(_timer);
	LOCK_FUNCTION(_timerUpdate);
	install_int_ex(_timerUpdate, BPS_TO_TIMER(100));

	unsigned int _fpsLast = 0;
	int _fpsCount = 0;
	int _fps = 0;
	unsigned int logicLast = 0;
	
	OmfgGUI::AllegroRenderer renderer;
/*
	while (menu)
	{
	    if (key[KEY_ESC])
	    {
	        menu = false;
	        key[KEY_ESC] = 0;
	    }
	    rectfill(gfx.buffer, 0, 0, 319, 239, 0);
	    top.keys((bool*)key);
	    top.render();
	    blit(gusLogo, gfx.buffer, 0, 0, 319 - gusLogo->w, 239 - gusLogo->h, gusLogo->w, gusLogo->h);
		gfx.updateScreen();
	}
*/

	//main game loop
	while (!quit)
	{

		while ( logicLast+1 <= _timer )
		{
			if ( game.isLoaded() && game.level.isLoaded() )
			{
				for ( ObjectsList::Iterator iter = game.objects.begin(); (bool)iter; ++iter)
				{
					(*iter)->think();
				}
				
				for ( list<BasePlayer*>::iterator iter = game.players.begin(); iter != game.players.end(); iter++)
				{
					(*iter)->think();
				}
			}
			
			sfx.think(); // WARNING: THIS ¡MUST! BE PLACED BEFORE THE OBJECT DELETE LOOP
			
			for ( list<BasePlayer*>::iterator iter = game.players.begin(); iter != game.players.end();)
			{
				if ( (*iter)->deleteMe )
				{
					for ( ObjectsList::Iterator objIter = game.objects.begin(); (bool)objIter; ++objIter)
					{
						(*objIter)->removeRefsToPlayer(*iter);
					}
					(*iter)->removeWorm();
					list<BasePlayer*>::iterator tmp = iter;
					++iter;
					delete *tmp;
					game.players.erase(tmp);
				}else
					++iter;
			}

			for ( ObjectsList::Iterator iter = game.objects.begin();  (bool)iter; )
			{
				if ( (*iter)->deleteMe )
				{
					ObjectsList::Iterator tmp = iter;
					++iter;
					delete *tmp;
					game.objects.erase(tmp);
				}else	++iter;
			}
			
#ifndef DISABLE_ZOIDCOM
			network.update();
#endif

			console.checkInput();
			console.think();
			
			logicLast+=1;
		}
		
#ifdef WINDOWS
		Sleep(0);
#else
		rest(0);
#endif
		//Update FPS
		if (_fpsLast + 100 <= _timer)
		{
			_fps = _fpsCount;
			_fpsCount = 0;
			_fpsLast = _timer;
			
			//console.addLogMsg(cast<string>(_fps));
		}
		
		if ( game.isLoaded() && game.level.isLoaded() )
		{
			for ( list<BasePlayer*>::iterator iter = game.players.begin(); iter != game.players.end(); iter++)
			{
				(*iter)->render();
			}
			
			//debug info
			if (showDebug)
			{
				game.infoFont->draw(gfx.buffer, "OBJECTS: " + cast<string>(game.objects.size()), 5, 10, 0);
				game.infoFont->draw(gfx.buffer, "PLAYERS: " + cast<string>(game.players.size()), 5, 15, 0);
				game.infoFont->draw(gfx.buffer, "PING:    " + cast<string>(network.getServerPing()), 5, 20, 0);
			}
		}
		
		//show fps
		if (showFps)
		{
			game.infoFont->draw(gfx.buffer, "FPS: " + cast<string>(_fps), 5, 5, 0);
		}
		_fpsCount++;

		OmfgGUI::menu.render(&renderer);
		console.render(gfx.buffer);

		EACH_CALLBACK(i, afterRender)
		{
			lua.callReference(*i);
		}
		
		gfx.updateScreen();
	}
	
	
	game.unload();
	console.shutDown();
	sfx.shutDown();
	gfx.shutDown();
	allegro_exit();

	return(0);
}
END_OF_MAIN();

