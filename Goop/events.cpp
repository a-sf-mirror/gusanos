#include "events.h"

#include "base_worm.h"
#include "base_object.h"
#include "base_action.h"
//#include "game_actions.h"
#include "util/macros.h"
#include "game.h"

#include <vector>
#include <string>
#include <map>
#include <stdexcept>

using namespace std;

Event::Event(int flags_)
: flags(flags_)
{
}

Event::~Event()
{
	/*vector<BaseAction*>::iterator i;
	for ( i = actions.begin(); i != actions.end(); i++)*/
	foreach(i, actions)
	{
		delete *i;
	}
}

bool Event::addAction( const string& name, const vector<string>& params )
{
	map<string, BaseAction*(*)( const std::vector< std::string > &) >::iterator tempAction = game.actionList.find(name);
	//HashTable<string, BaseAction*(*)( const std::vector< std::string > &) >::iterator tempAction = game.actionList.find(name);
	if ( tempAction != game.actionList.end() )
	{
		BaseAction* action = tempAction->second(params);
		
		if(((action->flags & BaseAction::RequiresObject) && !(flags & ProvidesObject))
		|| ((action->flags & BaseAction::RequiresObject2) && !(flags & ProvidesObject2))
		|| ((action->flags & BaseAction::RequiresWeapon) && !(flags & ProvidesWeapon)))
		{
			delete action;
			cerr << "The event does not provide the requirements of the action \"" << name << "\"" << endl;
			return false;
		}
		else
		{
			actions.push_back( action );
			return true;
		}
	}
	else
	{
		cerr << "Action with name \"" << name << "\" does not exist" << endl;
		return false;
	}
}

void Event::run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	if(((flags & ProvidesObject) && !object)
	|| ((flags & ProvidesObject2) && !object2)
	|| ((flags & ProvidesWeapon) && !weapon))
		throw std::runtime_error("The event doesn't provide what it's specified to provide");
		
	ActionParams params(object, object2, worm, weapon);
	for ( vector<BaseAction*>::iterator action = actions.begin(); action != actions.end(); action++)
	{
		(*action)->run( params );
	}
}
