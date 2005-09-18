#include "events.h"

#include "base_worm.h"
#include "base_object.h"
#include "base_action.h"
//#include "game_actions.h"
#include "game.h"

#include <vector>
#include <string>
#include <map>

using namespace std;

Event::Event()
{
}

Event::~Event()
{
	vector<BaseAction*>::iterator i;
	for ( i = actions.begin(); i != actions.end(); i++)
	{
		delete *i;
	}
}

void Event::addAction( const string& name, const vector<string>& params )
{
	map<string, BaseAction*(*)( const std::vector< std::string > &) >::iterator tempAction = game.actionList.find(name);
	//HashTable<string, BaseAction*(*)( const std::vector< std::string > &) >::iterator tempAction = game.actionList.find(name);
	if ( tempAction != game.actionList.end() )
	{
		actions.push_back( tempAction->second(params) );
	}
}

void Event::run( BaseObject *object, BaseObject *object2, BaseWorm *worm, Weapon *weapon )
{
	for ( vector<BaseAction*>::iterator action = actions.begin(); action != actions.end(); action++)
	{
		(*action)->run( object, object2, worm, weapon );
	}
}
