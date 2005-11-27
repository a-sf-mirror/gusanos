#ifndef EVENTS_H
#define EVENTS_H

//#include "base_action.h"

#include <string>
#include <vector>

class Weapon;
class BaseWorm;
class BaseObject;
class BaseAction;

struct Event
{
public:
	enum Flags
	{
		ProvidesObject = (1<<0),
		ProvidesObject2 = (1<<1),
		ProvidesWeapon = (1<<2),
	};
		
	Event(int flags_);
	Event(std::vector<BaseAction*>&);
	virtual ~Event();

	bool addAction( const std::string& name, const std::vector<std::string>& params );
	void swapActionList(std::vector<BaseAction*>& b); //TODO
	void run( BaseObject *object, BaseObject *object2 = NULL, BaseWorm *worm = NULL, Weapon *weapon = NULL );
	
	//private:
	
	std::vector<BaseAction*> actions;
	int flags;
};

#endif  // _PART_EVENTS_H_
