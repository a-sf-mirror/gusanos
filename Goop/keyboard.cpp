#include "keyboard.h"
#include <algorithm> // For std::find

#include <allegro.h>
#include <list>

using namespace std;

////////////////////////////PUBLIC//////////////////////////////

int KeyHandler::keyMap[KEY_MAX];
int KeyHandler::shiftCharMap[KEY_MAX]; // The shift map
int KeyHandler::capsCharMap[KEY_MAX]; // The caps lock map
int KeyHandler::charMap[KEY_MAX]; // The character map

//=========================LIFECYCLE==========================//

KeyHandler::KeyHandler()
{
	for(int i = 0; i < KEY_MAX; ++i)
	{
		oldKeys[i] = false;
	}
}

KeyHandler::~KeyHandler()
{
	
}

//=========================INTERFACE==========================//

void KeyHandler::init()
{
	install_keyboard();
	
	// Map as identity by default
	for(int i = 0; i < KEY_MAX; ++i)
	{
		keyMap[i] = i;
		shiftCharMap[i] = scancode_to_ascii(i);
		capsCharMap[i] = i;
		charMap[i] = scancode_to_ascii(i);
	}
	
	keyboard_ucallback = keyMapCallback; // Install key mapping callback
}

void KeyHandler::shutDown()
{
	remove_keyboard();
}

void KeyHandler::pollKeyboard()
{
	for (int i = 0; i < KEY_MAX; ++i)
	{
		if (getKey(i) && !oldKeys[i])
		{
			addEvent(KEY_EVENT_PRESS,i);
			oldKeys[i]=true;
		}
		else if (!getKey(i) && oldKeys[i])
		{
			addEvent(KEY_EVENT_RELEASE,i);
			oldKeys[i]=false;
		}
	}
}

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
}

/*static*/ int KeyHandler::keyMapCallback(int, int *scancode)
{
	// Translate scancode
	int newScancode = mapKey(*scancode);

	*scancode = newScancode;

	// Map the key to a character and return it
	if(getKey(KEY_LSHIFT) || getKey(KEY_RSHIFT))
		return  shiftCharMap[newScancode]; // Use shift map
	else
		return  charMap[newScancode]; // Use regular map
		
	//TODO: Add caps-lock and ctrl tables
}

/*static*/ int KeyHandler::getKey(int k)
{
	return key[mapKey(k)];
}

/*static*/ int KeyHandler::mapKey(int key)
{
	return keyMap[key];
}

/*static*/ int KeyHandler::reverseMapKey(int key)
{
	int *k = std::find(&keyMap[0], &keyMap[KEY_MAX], key);
	return static_cast<int>(k - keyMap);
}

/*static*/ void KeyHandler::swapKeyMapping(int keyA, int keyB)
{
	// Find what hardware key maps to the software keys to swap
	int keyAmap = reverseMapKey(keyA);
	int keyBmap = reverseMapKey(keyB);
	
	// Swap them
	keyMap[keyAmap] = keyB;
	keyMap[keyBmap] = keyA;
}

/*static*/ void KeyHandler::setShiftCharacter(int key, int character)
{
	shiftCharMap[key] = character;
}

/*static*/ void KeyHandler::setCharacter(int key, int character)
{
	charMap[key] = character;
}

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
}



