#include "consoleItem.h"

ConsoleItem::ConsoleItem(bool locked)
{
	m_locked = locked;
}

bool ConsoleItem::isLocked()
{
	return m_locked;
}

