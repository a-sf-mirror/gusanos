#include "part_events.h"

#include "base_object.h"
#include "game_actions.h"
#include "game.h"

#include <vector>
#include <string>
#include <map>

using namespace std;

PartEvent::PartEvent()
{
}

PartEvent::~PartEvent()
{
	vector<BaseAction*>::iterator i;
	for ( i = actions.begin(); i != actions.end(); i++)
	{
		delete *i;
	}
}

void PartEvent::addAction( const string& name, const vector<string>& params )
{
	map<string, BaseAction*(*)( const std::vector< std::string > &) >::iterator tempAction = game.actionList.find(name);
	if ( tempAction != game.actionList.end() )
	{
		actions.push_back( tempAction->second(params) );
	}
}

void PartEvent::run( BaseObject *object )
{
	for ( vector<BaseAction*>::iterator action = actions.begin(); action != actions.end(); action++)
	{
		(*action)->run( object );
	}
}
