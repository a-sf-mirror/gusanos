#ifndef KEYBOARD_h
#define KEYBOARD_h

#include <allegro.h>
#include <list>

#define KEY_EVENT_NONE	0
#define KEY_EVENT_PRESS	1
#define KEY_EVENT_RELEASE 2

#define BUFF_SIZE 64

struct KeyEvent
{
	int	type;
	char	key;
};

class KeyHandler
{
	public:
	
	KeyHandler(void);
	~KeyHandler(void);
	
	void init();
	void shutDown();
	void pollKeyboard();
	KeyEvent getEvent();

	static int keyMapCallback(int key, int *scancode);
	static int mapKey(int key);
	static int getKey(int k);
	static int reverseMapKey(int key);
	static void swapKeyMapping(int keyA, int keyB);
	static void setShiftCharacter(int key, int character);
	static void setCharacter(int key, int character);
	
	private:
	
	static int keyMap[KEY_MAX]; // The keymap
	static int charMap[KEY_MAX]; // The character map
	static int shiftCharMap[KEY_MAX]; // The shift map
	static int capsCharMap[KEY_MAX]; // The caps lock map
	
	std::list<KeyEvent> events;
	
	bool oldKeys[KEY_MAX]; // KEY_MAX is defined by allegro (usually 119)
	
	void addEvent(int type, char key);
	
};

#endif  // _KEYBOARD_h_
