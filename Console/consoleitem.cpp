#include "consoleitem.h"

ConsoleItem::ConsoleItem(bool locked)
: temp(false), m_locked(locked)
{
	
}

bool ConsoleItem::isLocked()
{
	return m_locked;
}

