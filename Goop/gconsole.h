#ifndef gconsole_h
#define gconsole_h

// SYSTEM INCLUDES
//

// PROJECT INCLUDES
//
#include "keyboard.h"
#include "console.h"

// LOCAL INCLUDES
//
#include <string>
#include <list>
#include <map>

// FORWARD REFERENCES
//
class ConsoleItem;

class GConsole : public Console
{
	public:

	void init();
	void shutDown();
	void checkInput();
	void render(BITMAP *where);

	private:
	
	KeyHandler keyHandler;
};

extern GConsole console;

#endif  // _gconsole_h_
