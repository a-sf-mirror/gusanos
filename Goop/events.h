#ifndef EVENTS_H
#define EVENTS_H

#include "base_action.h"

#include <string>
#include <vector>

class Weapon;
class Worm;
class BaseObject;

struct Event
{
	public:
		
	Event();
	~Event();

	void addAction( const std::string& name, const std::vector<std::string>& params );
	void run( BaseObject *object, BaseObject *object2 = NULL, Worm *worm = NULL, Weapon *weapon = NULL );
	
	private:
	
	std::vector<BaseAction*> actions;
};

#endif  // _PART_EVENTS_H_
