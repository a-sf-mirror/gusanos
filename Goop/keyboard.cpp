#include "keyboard.h"

#include <allegro.h>
#include <list>

using namespace std;

////////////////////////////PUBLIC//////////////////////////////

//=========================LIFECYCLE==========================//

KeyHandler::KeyHandler()
{
	for(int i = 0; i < KEY_MAX; ++i)
	{
		oldKeys[i] = false;
	}
};

KeyHandler::~KeyHandler()
{
	
};

//=========================INTERFACE==========================//

void KeyHandler::init()
{
	install_keyboard();
};

void KeyHandler::shutDown()
{
	remove_keyboard();
};

void KeyHandler::pollKeyboard()
{
	for (int i = 0; i < KEY_MAX; ++i)
	{
		if (key[i] && !oldKeys[i])
		{
			addEvent(KEY_EVENT_PRESS,i);
			oldKeys[i]=true;
		}
		else if (!key[i] && oldKeys[i])
		{
			addEvent(KEY_EVENT_RELEASE,i);
			oldKeys[i]=false;
		};
	}
};

KeyEvent KeyHandler::getEvent()
{
	KeyEvent event;
	event.type = KEY_EVENT_NONE;
	if (events.size() > 0)
	{
		event=events.back();
		events.pop_back();
	}
	return event;
};


////////////////////////////PRIVATE//////////////////////////////

void KeyHandler::addEvent(int type, char key)
{
	if (events.size() < BUFF_SIZE)
	{
		KeyEvent event;
		event.type = type;
		event.key = key;
		events.push_back(event);
	}
};



