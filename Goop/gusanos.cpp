#include <allegro.h>

#include "gconsole.h"
#include "resource_list.h"
#include "sprite.h"
#include "font.h"
#include "menu_window.h"
#include "vec.h"
#include "level.h"
#include "game.h"
#include "viewport.h"
#include "test_particle.h"
#include "worm.h"
//#include "text.h"

#include <string>

using namespace std;

Worm *worm;

bool forward = false;
bool quit = false;

string rightStart(const list<string> &args)
{
	worm->moveRightStart();
	return "";
}

string rightStop(const list<string> &args)
{
	worm->moveRightStop();
	return "";
}

string leftStart(const list<string> &args)
{
	worm->moveLeftStart();
	return "";
}

string leftStop(const list<string> &args)
{
	worm->moveLeftStop();
	return "";
}

string jumpStart(const list<string> &args)
{
	worm->jumpStart();
	return "";
}

string jumpStop(const list<string> &args)
{
	worm->jumpStop();
	return "";
}


string Exit(const list<string> &args)
{
	quit = true;
	return "";
}

int main(int argc, char **argv)
{
	game.init();

	
	
	Sprite *sprite = spriteList.load("sprite.bmp");
	Font *tempFont = fontList.load("minifont.bmp");
	
	
	console.registerCommand("+MOVELEFT", leftStart);
	console.registerCommand("-MOVELEFT", leftStop);
	console.registerCommand("+MOVERIGHT", rightStart);
	console.registerCommand("-MOVERIGHT", rightStop);
	console.registerCommand("+JUMP", jumpStart);
	console.registerCommand("-JUMP", jumpStop);
	console.registerCommand("QUIT", Exit);
	
	console.parseLine("BIND A +MOVELEFT; BIND D +MOVERIGHT; BIND G +JUMP");
	
	if ( gameLoad<Level>("bleed",game.level) )
	{
		console.addLogMsg("MAP LOADED SUCCESFULLY");
	}else
		console.addLogMsg("COULDNT LOAD THE MAP");
	//allegro_message("%d",consoleTest);
	
	BITMAP *buffer;
	

	MenuWindow menu;

  
	buffer = create_bitmap(320,240);
	
	Viewport testViewport;
	Viewport testViewport2;
	
	testViewport.setDestination(buffer,0,0,320,240);
	testViewport2.setDestination(buffer,100,50,100,100);
	
	for (int i = 0; i < 200 ; i++)
	{
		game.objects.push_back( new TestParticle );
	}
	
	worm = new Worm();
	game.objects.push_back(worm);
	
	
	int x,y;
	int moo=0;
	int moox,mooy;
	while (!quit)
	{
		
		list<BaseObject*>::iterator iter;
		for ( iter = game.objects.begin(); iter != game.objects.end(); iter++)
		{
			(*iter)->think();
		}
		
		console.checkInput();
		
		clear_bitmap(buffer);
		
		//level.draw(buffer,0,object.pos.x);
		testViewport.render();
		testViewport2.interpolateTo(worm->getPos(),0.1);
		testViewport2.setPos(worm->getPos().x - 50, worm->getPos().y - 50);
		testViewport2.render();
		
		console.think();
		console.render(buffer);
		
		vsync();
		
		blit(buffer,screen,0,0,0,0,320,240);
	}
	
	console.shutDown();
	//level.unload();
	allegro_exit();

	return(0);
}
END_OF_MAIN();

