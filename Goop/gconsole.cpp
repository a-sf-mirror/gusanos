#include "gconsole.h"
#include "keyboard.h"

#include <allegro.h>

using namespace std;

/////////////////////////////// Console //////////////////////////////////////

//============================= LIFECYCLE ====================================

//============================= INTERFACE ====================================

void GConsole::init()
{
	keyHandler.init();
}

void GConsole::shutDown()
{
	keyHandler.shutDown();
}

void GConsole::render(BITMAP* where)
{
	int textIndex = 0;
	list<std::string>::iterator msg = log.end();
	while (msg != log.begin())
	{
		msg--;
		textout_ex(where, font, (*msg).c_str(), 5, where->h - 10 - textIndex * 10, makecol(255,255,255), -1);
		textIndex++;
	}
}

void GConsole::checkInput()
{
	keyHandler.pollKeyboard();
	
	KeyEvent event = keyHandler.getEvent();

	while (event.type != KEY_EVENT_NONE)
	{
		if (event.type == KEY_EVENT_PRESS)
		{
			analizeKeyEvent(true, event.key);
		}else
		{
			analizeKeyEvent(false, event.key);
		}
		event = keyHandler.getEvent();
	}
}




//============================= PRIVATE ======================================




GConsole console;

