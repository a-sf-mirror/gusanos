#include <allegro.h>

#include "console.h"

struct Object
{
	float x;
	float y;
};

int main(int argc, char **argv)
{
	Console console;
	KeyHandler keys;
	KeyEvent event;
	
	Object object;
	
	allegro_init();
	keys.init();
	
	int consoleTest;
	int pooo;
	
	console.registerIntVariable(&consoleTest, "TEST", 100);
	console.registerIntVariable(&pooo, "POO", 20);
	allegro_message("%d",consoleTest);
	console.setVariableValue("TEST", "10");
	console.setVariableValue("POO", "15");
	allegro_message("%d",consoleTest);
	allegro_message("%d",pooo);
	
	console.parseLine("moo caco;mooish;moootonic");
	
	bool quit = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	
	BITMAP *buffer;
	
	set_color_depth(16);

	set_gfx_mode(GFX_AUTODETECT, 320, 240, 0, 0);
			
	if(set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
		set_display_switch_mode(SWITCH_BACKGROUND);
  
	buffer = create_bitmap(320,240);
	
	while (!quit)
	{
		keys.pollKeyboard();
		event=keys.getEvent();
		
		if (up) object.y -= 1;
		if (down) object.y += 1;
		if (left) object.x -= 1;
		if (right) object.x += 1;
			
		clear_bitmap(buffer);
		putpixel(buffer,object.x,object.y,makecol(255,255,255));
		
		vsync();
		
		blit(buffer,screen,0,0,0,0,320,240);
			
		while (event.type != KEY_EVENT_NONE)
		{
			if (event.key == KEY_W)
			{
				if (event.type == KEY_EVENT_PRESS)
					up = true;
				else
					up = false;
			}
			if (event.key == KEY_S)
			{
				if (event.type == KEY_EVENT_PRESS)
					down = true;
				else
					down = false;
			}
			if (event.key == KEY_D)
			{
				if (event.type == KEY_EVENT_PRESS)
					right = true;
				else
					right = false;
			}
			if (event.key == KEY_A)
			{
				if (event.type == KEY_EVENT_PRESS)
					left = true;
				else
					left = false;
			}
			if (event.type == KEY_EVENT_RELEASE && event.key==KEY_Q)
				quit=true;
			event=keys.getEvent();
		};
	};

	return(0);
}
END_OF_MAIN();

