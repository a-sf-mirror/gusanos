#ifndef CONSOLE_ITEM_H
#define CONSOLE_ITEM_H

#include <string>
#include <list>

class ConsoleItem
{
	public:
		
	ConsoleItem(bool locked = true);

	virtual std::string invoke(const std::list<std::string> &args) = 0;
	bool isLocked();
	
	private:
		
	bool m_locked;
};

#endif  // _CONSOLE_ITEM_H_
