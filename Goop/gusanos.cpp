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
				game.infoFont->draw(gfx.buffer, "OBJECTS: \01303" + cast<string>(game.objects.size()), 5, 10, 0, 255, 255, 255, 255, Font::Formatting);
				game.infoFont->draw(gfx.buffer, "PLAYERS: \01303" + cast<string>(game.players.size()), 5, 15, 0, 255, 255, 255, 255, Font::Formatting);
				game.infoFont->draw(gfx.buffer, "PING:    \01303" + cast<string>(network.getServerPing()), 5, 20, 0, 255, 255, 255, 255, Font::Formatting);
			}
			/*
			int y = 235;
			int w = 0;
			std::list<ScreenMessage>::reverse_iterator rmsgiter = game.messages.rbegin();
			for(; rmsgiter != game.messages.rend() && y > 150;
			    ++rmsgiter)
			{
				ScreenMessage const& msg = *rmsgiter;
				
				pair<int, int> dim = game.infoFont->getDimensions(msg.str, 0, Font::Formatting);
				
				y -= dim.second;
				
				if(dim.first > w)
					w = dim.first;
			}
			
			rectfill_blend(gfx.buffer, 3, y-2, 3+w+5, 237, 0, 130);
			
			for(std::list<ScreenMessage>::iterator msgiter = rmsgiter.base();
			    msgiter != game.messages.end() && y < 240;
			    ++msgiter)
			{
				ScreenMessage const& msg = *msgiter;
				
				pair<int, int> dim = game.infoFont->getDimensions(msg.str, 0, Font::Formatting);
				
				int fact = 150;
				if(msg.timeOut < 100)
					fact = msg.timeOut * 150 / 100;
					
				Font::CharFormatting format;
				switch(msg.type)
				{
					case ScreenMessage::Chat:
						format.cur.color = Font::Color(255, 255, 255);
					break;
					
					case ScreenMessage::Death:
						format.cur.color = Font::Color(200, 255, 200);
					break;
				}
				
				game.infoFont->draw(gfx.buffer, msg.str, 5, y, format, 0, fact, Font::Formatting | Font::Shadow);
				
				y += dim.second;
			}
			*/
			
			int miny = 150;
			int maxw = 160;
			int y = 235;
			int w = 0;
			
			std::list<ScreenMessage>::reverse_iterator rmsgiter = game.messages.rbegin();
			
			for(;
		    rmsgiter != game.messages.rend() && y > miny;
		    ++rmsgiter)
			{
				ScreenMessage const& msg = *rmsgiter;
				
				string::const_iterator b = msg.str.begin(), e = msg.str.end(), n;
				
				do
				{
					pair<int, int> dim;
					n = game.infoFont->fitString(b, e, maxw, dim, 0, Font::Formatting);
					if(n == b)
						break;
					b = n;
					y -= dim.second;
					
					if(dim.first > w)
						w = dim.first;
				}
				while(b != e);
			}
			
			//rectfill_blend(gfx.buffer, 3, y-2, 3+w+5, 237, 0, 130);
			
			for(std::list<ScreenMessage>::iterator msgiter = rmsgiter.base();
			    msgiter != game.messages.end();
			    ++msgiter)
			{
				ScreenMessage const& msg = *msgiter;
				
				string::const_iterator b = msg.str.begin(), e = msg.str.end(), n;
				
				int fact = 255;
				if(msg.timeOut < 100)
					fact = msg.timeOut * 255 / 100;
				
				Font::CharFormatting format;
				switch(msg.type)
				{
					case ScreenMessage::Chat:
						format.cur.color = Font::Color(255, 255, 255);
					break;
					
					case ScreenMessage::Death:
						format.cur.color = Font::Color(200, 255, 200);
					break;
				}
				
				do
				{
					pair<int, int> dim;
					n = game.infoFont->fitString(b, e, maxw, dim, 0, Font::Formatting);
					if(n == b)
						break;
					game.infoFont->draw(gfx.buffer, b, n, 5, y, format, 0, fact, Font::Formatting | Font::Shadow);
					y += dim.second;
					
					b = n;
				}
				while(b != e);
			}
		}

		
		//show fps
		if (showFps)
		{
			game.infoFont->draw(gfx.buffer, "FPS: \01303" + cast<string>(_fps), 5, 5, 0, 255, 255, 255, 255, Font::Formatting);
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

