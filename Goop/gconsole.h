#ifndef gconsole_h
#define gconsole_h

#include <console.h>
#include "font.h"
#include "sprite.h"

#include <allegro.h>

#include <list>
#include <string>
#include <list>
#include <map>

class GConsole : public Console
{
	public:

	GConsole();
	
	void init();
	void shutDown();
	void loadResources();
	void checkInput();
	void render(BITMAP *where, bool fullScreen = false);
	void think();
	
	bool eventPrintableChar(char c);
	bool eventKeyDown(int k);
	bool eventKeyUp(int k);
	
	virtual std::string::const_iterator fitString(
		std::string::const_iterator b,
		std::string::const_iterator e);

	private:
	
	float m_pos;
	float speed;
	int height;
	int m_mode;
	
	//KeyHandler keyHandler;

	Font *m_font;
	Sprite *background;
	std::string m_inputBuff;
	int m_consoleKey;
	
	std::list< std::string > commandsLog;
	std::list< std::string >::iterator currentCommand;
	
	enum
	{
		CONSOLE_MODE_INPUT,
		CONSOLE_MODE_BINDINGS
	};
};

std::string bindCmd(const std::list<std::string> &args);

extern GConsole console;

#endif  // _gconsole_h_
