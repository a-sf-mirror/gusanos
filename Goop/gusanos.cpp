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
#include "part_type.h"
#include "particle.h"
#include "worm.h"
#include "text.h"
#include "gfx.h"
#include "sfx.h"

#include <string>
#include <vector>

#include <fmod/fmod.h>

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

bool aimUp = false;

string aimUpStart(const list<string> &args)
{
	aimUp = true;
	return "";
}

string aimUpStop(const list<string> &args)
{
	aimUp = false;
	return "";
}

bool aimDown = false;

string aimDownStart(const list<string> &args)
{
	aimDown = true;
	return "";
}

string aimDownStop(const list<string> &args)
{
	aimDown = false;
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
	
	float aimSpeed;
	
	console.registerFloatVariable("CL_TEMP_AIM_SPEED", &aimSpeed, 1.8);
	
	console.registerCommand("+MOVELEFT", leftStart);
	console.registerCommand("-MOVELEFT", leftStop);
	console.registerCommand("+MOVERIGHT", rightStart);
	console.registerCommand("-MOVERIGHT", rightStop);
	console.registerCommand("+AIMUP", aimUpStart);
	console.registerCommand("-AIMUP", aimUpStop);
	console.registerCommand("+AIMDOWN", aimDownStart);
	console.registerCommand("-AIMDOWN", aimDownStop);
	console.registerCommand("+JUMP", jumpStart);
	console.registerCommand("-JUMP", jumpStop);
	console.registerCommand("QUIT", Exit);
	
	console.parseLine("BIND A +MOVELEFT; BIND D +MOVERIGHT; BIND G +JUMP; BIND W +AIMUP; BIND S +AIMDOWN; BIND F12 SCREENSHOT");
	
	if ( gameLoad<Level>("bleed",game.level) )
	{
		console.addLogMsg("MAP LOADED SUCCESFULLY");
	}else
		console.addLogMsg("COULDNT LOAD THE MAP");
	
	PartType* testType = partTypeList.load("test.obj");

	MenuWindow menu;
	
	Viewport testViewport;
	Viewport testViewport2;
	
	testViewport.setDestination(gfx.buffer,0,0,320,240);
	testViewport2.setDestination(gfx.buffer,110,70,100,100);
	
	for (int i = 0; i < 1 ; i++)
	{
		game.objects.push_back( new Particle(testType) );
	}
	
	worm = new Worm();
	game.objects.push_back(worm);
	
	int x,y;
	int moo=0;
	int moox,mooy;
	
	while (!quit)
	{
		
		list<BaseObject*>::iterator iter;
		
		if ( aimUp ) worm->addToAim(-aimSpeed);
		if ( aimDown ) worm->addToAim(aimSpeed);	

		for ( iter = game.objects.begin(); iter != game.objects.end(); iter++)
		{
			(*iter)->think();
		}
		
		console.checkInput();

		testViewport.interpolateTo(worm->getPos(),0.1);
		testViewport.render();
		
		console.think();
		console.render(gfx.buffer);
		
		gfx.updateScreen();
		
		float pos[3] = { worm->getPos().x, worm->getPos().y, -20 };
		FSOUND_3D_Listener_SetAttributes(pos,NULL,0,0,1,0,1,0);
		FSOUND_Update();
		
		//sfx.updateChanPositions();
		
		//sfx.checkForDeletedObjects();
		
		for ( iter = game.objects.begin(); iter != game.objects.end(); )
		{
			if ( (*iter)->deleteMe )
			{
				list<BaseObject*>::iterator tmp = iter;
				iter++;
				delete *tmp;
				game.objects.erase(tmp);
			}else	iter++;
		}
		

	}
	
	
	console.shutDown();
	sfx.shutDown();
	gfx.shutDown();
	//level.unload();
	allegro_exit();

	return(0);
}
END_OF_MAIN();

