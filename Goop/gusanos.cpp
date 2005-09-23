#include <allegro.h>

#include "gconsole.h"
#include "resource_list.h"
#include "sprite.h"

#include "vec.h"
#include "level.h"
#include "game.h"
#include "part_type.h"
#include "particle.h"
#include "worm.h"
#include "player.h"
#include "text.h"
#ifndef DEDSERV
#include "viewport.h"
#include "font.h"
#include "gfx.h"
#include "sfx.h"
#include "menu.h"
#include "distortion.h"
#include "keyboard.h"
#endif
#include "player_ai.h"
#include "network.h"


#include "script.h"
#include "glua.h"

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

	//OmfgGUI::menu.testParseXML();

	console.registerVariables()
		("CL_SHOWFPS", &showFps, 1)
		("CL_SHOWDEBUG", &showDebug, 1)
	;
	
	console.registerCommands()
		(string("QUIT"), Exit)
	;
	
	console.parseLine("BIND F12 SCREENSHOT");

#ifndef DEDSERV
	OmfgGUI::menu.clear();
	OmfgGUI::AllegroRenderer renderer;
#endif
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
	
#ifndef DEDSERV
	console.executeConfig("autoexec.cfg");
#else
	console.executeConfig("autoexec-ded.cfg");
#endif

	//main game loop
	while (!quit)
	{

		while ( logicLast+1 <= _timer )
		{
			
#ifdef USE_GRID
			for ( Grid::iterator iter = game.objects.beginAll(); iter;)
			{
				if(iter->deleteMe)
					iter.erase();
				else
					++iter;
			}
#else
			for ( ObjectsList::Iterator iter = game.objects.begin();  iter; )
			{
				if ( (*iter)->deleteMe )
				{
					ObjectsList::Iterator tmp = iter;
					++iter;
					delete *tmp;
					game.objects.erase(tmp);
				}else	++iter;
			}
#endif
			
			if ( game.isLoaded() && game.level.isLoaded() )
			{
#ifdef USE_GRID
				
				for ( Grid::iterator iter = game.objects.beginAll(); iter; ++iter)
				{
					iter->think();
					game.objects.relocateIfNecessary(iter);
				}
				
				game.objects.flush(); // Insert all new objects
#else
				for ( ObjectsList::Iterator iter = game.objects.begin(); (bool)iter; ++iter)
				{
					(*iter)->think();
				}
#endif
				
				for ( list<BasePlayer*>::iterator iter = game.players.begin(); iter != game.players.end(); iter++)
				{
					(*iter)->think();
				}
				
				game.think();
			}
			
#ifndef DEDSERV
			sfx.think(); // WARNING: THIS ¡MUST! BE PLACED BEFORE THE OBJECT DELETE LOOP
#endif
			
			for ( list<BasePlayer*>::iterator iter = game.players.begin(); iter != game.players.end();)
			{
				if ( (*iter)->deleteMe )
				{
#ifdef USE_GRID
					for (Grid::iterator objIter = game.objects.beginAll(); objIter; ++objIter)
					{
						objIter->removeRefsToPlayer(*iter);
					}
#else
					for ( ObjectsList::Iterator objIter = game.objects.begin(); (bool)objIter; ++objIter)
					{
						(*objIter)->removeRefsToPlayer(*iter);
					}
#endif
					(*iter)->removeWorm();
					list<BasePlayer*>::iterator tmp = iter;
					++iter;
					delete *tmp;
					game.players.erase(tmp);
				}else
					++iter;
			}



			
#ifndef DISABLE_ZOIDCOM
			network.update();
#endif

#ifndef DEDSERV
			console.checkInput();
#endif
			console.think();
			
			logicLast+=1;
		}
		
#ifdef WINDOWS
		Sleep(0);
#else
#ifndef DEDSERV
		rest(0);
#else
		rest(2);
#endif
#endif

#ifndef DEDSERV
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
				game.infoFont->drawFormatted(gfx.buffer, "OBJECTS: \01303" + cast<string>(game.objects.size()), 5, 10, 0);
				game.infoFont->drawFormatted(gfx.buffer, "PLAYERS: \01303" + cast<string>(game.players.size()), 5, 15, 0);
				game.infoFont->drawFormatted(gfx.buffer, "PING:    \01303" + cast<string>(network.getServerPing()), 5, 20, 0);
			}
			
			int y = 235;
			for(std::list<ScreenMessage>::reverse_iterator msgiter = game.messages.rbegin();
			    msgiter != game.messages.rend() && y > 0;
			    ++msgiter)
			{
				ScreenMessage const& msg = *msgiter;
				
				pair<int, int> dim = game.infoFont->getFormattedDimensions(msg.str);
				
				y -= dim.second;
				
				int fact = 255;
				if(msg.timeOut < 100)
					fact = msg.timeOut * 255 / 100;
					
				int r, g, b;
				switch(msg.type)
				{
					case ScreenMessage::Chat:
						r = 255; g = 255; b = 255;
					break;
					
					case ScreenMessage::Death:
						r = 0; g = 255; b = 0;
					break;
				}
				
				game.infoFont->drawFormatted(gfx.buffer, msg.str, 5, y, 0, r, g, b, fact, Font::Shadow);
			}
		}

		
		//show fps
		if (showFps)
		{
			game.infoFont->drawFormatted(gfx.buffer, "FPS: \01303" + cast<string>(_fps), 5, 5, 0);
		}
		_fpsCount++;


		OmfgGUI::menu.render(&renderer);
		console.render(gfx.buffer);

		EACH_CALLBACK(i, afterRender)
		{
			lua.callReference(*i);
		}
		
		gfx.updateScreen();
#endif
	}
	
	
	network.disconnect();
	network.shutDown();
	game.unload();
	console.shutDown();
#ifndef DEDSERV
	sfx.shutDown();
#endif
	gfx.shutDown();

	allegro_exit();

	return(0);
}
END_OF_MAIN();

