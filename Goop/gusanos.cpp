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
//#include "text.h"

#include <string>

using namespace std;

struct Object
{
	Vec pos;
	Vec spd;
	float x;
	float y;
};

bool forward = false;
bool quit = false;

string forwardStart(const list<string> &args)
{
	forward = true;
	return "";
}

string forwardEnd(const list<string> &args)
{
	forward = false;
	return "";
}

string Exit(const list<string> &args)
{
	quit = true;
	return "";
}

int main(int argc, char **argv)
{
	Object object;
	
	
	game.init();

	
	
	Sprite *sprite = spriteList.load("sprite.bmp");
	Font *tempFont = fontList.load("minifont.bmp");
	
	
	console.registerCommand("+FORWARD", forwardStart);
	console.registerCommand("-FORWARD", forwardEnd);
	console.registerCommand("QUIT", Exit);
	
	console.parseLine("moo caco; \"boooyouvoooo vo; asfa\" \" \";mooish  ");
	console.parseLine("TEST");
	console.parseLine("uga 1ish; uga   2ish;TEST 48");
	console.parseLine("TEST");
	console.parseLine("BIND A \"TEST 10\"");
	console.parseLine("BIND B TEST");
	console.parseLine("BIND W +FORWARD");
	
	
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
	
	for (int i = 0; i < 1000 ; i++)
	{
		game.objects.push_back( new TestParticle );
	}
	
	object.pos = Vec(0,0);
	
	int x,y;
	int moo=0;
	int moox,mooy;
	while (!quit)
	{
		if (forward) object.pos= object.pos + Vec(0.3,0);
		
		list<BaseObject*>::iterator iter;
		for ( iter = game.objects.begin(); iter != game.objects.end(); iter++)
		{
			(*iter)->think();
		}
		
		console.checkInput();
		
		clear_bitmap(buffer);
		
		textout_ex(buffer, font, "Console", 10, 10, makecol(255,255,255), -1);
		
		tempFont->draw(buffer,"abcdefghijklmnopqrst ABCDEFGHIJKLMNOPQRST", 10,20,0);
		
		//level.draw(buffer,0,object.pos.x);
		testViewport.render();
		if ( moo < 200 )
		{
			testViewport2.interpolateTo(moox,mooy,0.02);
			moo++;
		}else
		{
			moox=rand()%150;
			mooy=rand()%400;
			moo=0;
		}
		testViewport2.render();
		console.render(buffer);
		//menu.draw(buffer);
		
		putpixel(buffer, object.pos.x, object.pos.y, makecol(255,0,0));
		sprite->draw(buffer, 1, object.pos.x, object.pos.y);
		
		vsync();
		
		/*for (int i= 0; i<20; i++)
		{
		x = rand()%100;
		y = rand()%200;
		if ( level.getMaterial(x,y).worm_pass ) putpixel(buffer,x,y,makecol(255,255,0));
		}*/
		
		blit(buffer,screen,0,0,0,0,320,240);

	}
	
	console.shutDown();
	//level.unload();
	allegro_exit();

	return(0);
}
END_OF_MAIN();

