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

string ugauga(const list<string> &args)
{
	allegro_message((*args.begin()).c_str());
	return "Guarda boludo! ejecutaste \"uga\"";
}

string bind(const list<string> &args)
{
	string key;
	string action;
	
	key = *args.begin();
	action = *(++args.begin());
	console.bind(key, action);
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
	console.registerIntVariable("POO", &pooo, 20);
	console.registerCommand("uga", ugauga);
	console.registerCommand("BIND", bind);
	//allegro_message("%d",consoleTest);
	//console.setVariableValue("TEST", "10");
	//console.setVariableValue("POO", "15");
	//allegro_message("%d",consoleTest);
	//allegro_message("%d",pooo);
	
	console.parseLine("moo caco; \"boooyouvoooo vo; asfa\" \" \";mooish  ");
	console.parseLine("TEST");
	console.parseLine("uga 1ish; uga   2ish;TEST 48");
	console.parseLine("TEST");
	console.parseLine("BIND A \"TEST 10\"");
	console.parseLine("BIND B TEST");
	
	//allegro_message("%d",consoleTest);
	
	BITMAP *buffer;
	
	bool quit=false;
	
	set_color_depth(16);

	set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0);
			
	if(set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
		set_display_switch_mode(SWITCH_BACKGROUND);
  
	buffer = create_bitmap(320,240);
	
	while (true)
	{
			
		console.checkInput();
		
		clear_bitmap(buffer);
		
		textout_ex(buffer, font, "Console", 10, 10, makecol(255,255,255), -1);
		
		console.render(buffer);
		
		vsync();
		
		blit(buffer,screen,0,0,0,0,320,240);

	};

	return(0);
}
END_OF_MAIN();

