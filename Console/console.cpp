#include "console.h"
#include "variables.h"
#include "command.h"
#include "alias.h"
#include "text.h"
#include "consoleItem.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <cctype>


using namespace std;

/////////////////////////////// Console //////////////////////////////////////

//============================= LIFECYCLE ====================================

Console::Console()
{
	m_logMaxSize=256;
	m_MaxMsgLength=40;
}

Console::Console(int logMaxSize, int MaxMsgLength)
{
	m_logMaxSize=logMaxSize;
	m_MaxMsgLength=MaxMsgLength;
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

void Console::registerFloatVariable(const string &name, float* src, float defaultValue)
{
	if (!name.empty())
	{
		map<string, ConsoleItem*>::iterator tempItem = items.find(name);
		if (tempItem == items.end())
		{
			items[name] = new FloatVariable(src, name, defaultValue);
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

void Console::registerAlias(const std::string &name, const std::string &action)
{
	if (!name.empty())
	{
		map<string, ConsoleItem*>::iterator tempItem = items.find(name);
		if (tempItem == items.end())
		{
			items[name] = new Alias(this,name,action);
		}else if (!tempItem->second->isLocked())
		{
			items[name] = new Alias(this,name,action);
		}
	}
}

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
				addLogMsg("UNKNOWN COMMAND \"" + itemName + "\"" );
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
		
		string tmpmsg = msg;
		
		while (tmpmsg.length() > m_MaxMsgLength)
		{
			log.push_back(tmpmsg.substr(0,m_MaxMsgLength));
			tmpmsg=tmpmsg.substr(m_MaxMsgLength);
		}
		
		log.push_back(tmpmsg);
	}
}

void Console::analizeKeyEvent(bool state, char key)
{
	if (state == true)
		parseLine(bindTable.getBindingAction(key));
	else
		parseLine(bindTable.getBindingAction(key), true);
}

void Console::bind(char key, const string &action)
{
	bindTable.bind(key , action);
};

int Console::executeConfig(const string &filename)
{
	ifstream file;

	file.open(filename.c_str());
	if (file.is_open() && file.good())
	{
		string text2Parse;
		//...parse the file
		while (!file.eof())
		{
			getline(file,text2Parse);
			std::transform(text2Parse.begin(), text2Parse.end(), text2Parse.begin(), (int(*)(int)) toupper);
			parseLine(text2Parse);
		};
		file.close();
		
		return 1;
	};
	file.close();
	
	return 0;
};

string Console::autoComplete(const string &text)
{
	string returnText = text;
	
	if ( !text.empty() )
	{
		map<string, ConsoleItem*>::iterator item = items.lower_bound( text );
		if( item != items.end() )
		{
			if ( text == item->first.substr(0, text.length()) )
			{	
				map<string, ConsoleItem*>::iterator firstMatch = item;
				map<string, ConsoleItem*>::iterator lastMatch;
				
				while ( item != items.end() && text == item->first.substr(0, text.length() ) )
				{
					lastMatch = item;
					item++;
				}
				
				if (lastMatch == firstMatch)
				{
					returnText = firstMatch->first;
				}else
				{
					lastMatch++;
					bool differenceFound = false;
					int i = 0;
					while(!differenceFound)
					{
						i++;
						for (item = firstMatch; item != lastMatch ; item++)
						{
							if (firstMatch->first.substr(0, text.length() + i) != item->first.substr(0, text.length() + i) )
							{
								differenceFound = true;
							}
						}
					}
					returnText = firstMatch->first.substr(0, text.length() + i -1);
				}
			}
		}
	}
	return returnText;
}

void Console::listItems(const string &text)
{
	if ( !text.empty() )
	{
		// Find the first item that matches that text
		map<string, ConsoleItem*>::iterator item = items.lower_bound( text ); 
		if( item != items.end() && text == item->first.substr(0, text.length()) ) // If found
		{
			// Temp item to check if there is only 1 item matching the given text
			map<string, ConsoleItem*>::iterator tempItem = item; 
			tempItem++;
			
			// If the temp item is equal to the first item found it means that there are more than 1 items that match
			if ( tempItem != items.end() )
			if ( tempItem->first.substr(0, text.length() ) == item->first.substr(0, text.length()) )
			{
				
				addLogMsg("]");
				
				// Add a message with the name of all the matching items
				while ( item != items.end() && text == item->first.substr(0, text.length() ) )
				{
					addLogMsg(item->first);
					item++;
				}
			}
		}
	}
}

//============================= PRIVATE ======================================

