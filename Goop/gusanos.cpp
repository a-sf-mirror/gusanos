#include <allegro.h>

#include "gconsole.h"
//#include "text.h"

#include <string>

using namespace std;

struct Object
{
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
	
	allegro_init();
	console.init();
	
	int consoleTest;
	int pooo;
	
	console.registerIntVariable("TEST", &consoleTest, 100);
	console.registerIntVariable("POO1", &pooo, 20);
	console.registerIntVariable("POO2", &pooo, 20);
	console.registerIntVariable("POO3", &pooo, 20);
	console.registerIntVariable("POO4", &pooo, 20);
	console.registerIntVariable("POO5", &pooo, 20);
	console.registerIntVariable("POP1", &pooo, 20);
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
	
	//allegro_message("%d",consoleTest);
	
	BITMAP *buffer;
	
	set_color_depth(16);

	set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0);
			
	if(set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
		set_display_switch_mode(SWITCH_BACKGROUND);
  
	buffer = create_bitmap(320,240);
	
	object.x = 10;
	object.y = 10;
	
	while (!quit)
	{
		if (forward) object.x+=0.5;
		
		console.checkInput();
		
		clear_bitmap(buffer);
		
		textout_ex(buffer, font, "Console", 10, 10, makecol(255,255,255), -1);
		
		console.render(buffer);
		
		putpixel(buffer, object.x, object.y, makecol(255,0,0));
		
		vsync();
		
		blit(buffer,screen,0,0,0,0,320,240);

	}
	
	console.shutDown();
	allegro_exit();

	return(0);
}
END_OF_MAIN();

