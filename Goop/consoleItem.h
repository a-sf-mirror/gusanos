#ifndef consoleitem_h
#define consoleitem_h

#include <string>
#include <list>
#include <allegro.h>

class ConsoleItem
{
	public:

	virtual std::string invoke(const std::list<std::string> &args) = 0;
};

#endif  // _consoleitem_h_
