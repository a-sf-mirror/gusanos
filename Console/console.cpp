#include "console.h"

#include <allegro.h>

#include "variables.h"
#include "command.h"
#include "text.h"
#include "keys.h"

using namespace std;

/////////////////////////////// Console //////////////////////////////////////

//============================= LIFECYCLE ====================================

Console::Console()
{
	m_logMaxSize=256;
}

Console::~Console()
{
	// Delete the registered variables
	map<string, ConsoleItem*>::iterator tempvar = items.begin();
	while (tempvar != items.end())
	{
		delete tempvar->second;
		tempvar++;
	}
	items.clear();
}

//============================= INTERFACE ====================================

void Console::registerIntVariable(const string &name, int* src, int defaultValue)
{
	if (!name.empty())
	{
		map<string, ConsoleItem*>::iterator tempItem = items.find(name);
		if (tempItem == items.end())
		{
			items[name] = new IntVariable(src, name, defaultValue);
		}
	}
}

void Console::registerCommand(const std::string &name, std::string (*func)(const std::list<std::string>&))
{
	if (!name.empty())
	{
		map<string, ConsoleItem*>::iterator tempItem = items.find(name);
		if (tempItem == items.end())
		{
			items[name] = new Command(func);
		}
	}
}

/*void Console::setVariableValue(const string &name, const string &value)
{
	map<string, ConsoleItem*>::iterator tempItem = items.find(name);
	if (tempItem != items.end())
		tempItem->second->invoke(value);
}*/

void Console::parseLine(const string &text, bool parseRelease)
{
	string textToParse;
	string textInQueue = text;
	
	list< list<string> > argTree = text2Tree(text);
	
	list< list<string> >::iterator mainIter = argTree.begin();
	while ( mainIter != argTree.end() )
	{
		parse( (*mainIter), parseRelease );
		mainIter++;
	}

}

void Console::parse(list<string> &args, bool parseRelease)
{
	string itemName;
	string arguments;
	string retString;
	
	if (!args.empty())
	{
		itemName = *args.begin();
		if ( !parseRelease || (itemName[0] == '+') )
		{
			if (parseRelease) itemName[0]='-';
			map<string, ConsoleItem*>::iterator tempItem = items.find(itemName);
			if (tempItem != items.end())
			{
				args.pop_front();
				retString = tempItem->second->invoke(args);
				addLogMsg(retString);
			}else
			{
				addLogMsg("Unknown command \"" + itemName + "\"" );
			}
		}
	}
}

void Console::addLogMsg(const string &msg)
{
	if (!msg.empty())
	{
		if(log.size() >= m_logMaxSize)
			log.pop_front();
		log.push_back(msg);
	}
}

void Console::analizeKeyEvent(bool state, char key)
{
	if (state == true)
		parseLine(bindTable.getBindingAction(key));
	else
		parseLine(bindTable.getBindingAction(key), true);
}

void Console::bind(const std::string &key, const string &action)
{
	bindTable.bind(kName2Int(key), action);
};

//============================= PRIVATE ======================================

