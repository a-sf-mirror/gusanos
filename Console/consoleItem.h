#ifndef consoleItem_h
#define consoleItem_h

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

#endif  // _consoleItem_h_
