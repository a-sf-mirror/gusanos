#include "gconsole.h"
#include "keyboard.h"
#include "keys.h"
#include "font.h"

#include <allegro.h>

using namespace std;

#define TEST_KEY(k_, keyname_) if(k_ < 0) return "UNKNOWN KEY \"" + keyname_ + '"'

// Bind console command
string bindCmd(const list<string> &args)
{
	// <GLIP> Simplified a little, removed unused vars
	// and made it print out the help text when too
	// few arguments are passed.

	if (args.size() >= 2) 
	{
		std::list<string>::const_iterator arguments = args.begin();
		
		std::string const& keyName = *arguments++;
		int key = kName2Int(keyName);
		TEST_KEY(key, keyName);

		console.bind(key, *arguments++);
	
		return "";
	}
	
	return "BIND <KEY> [COMMAND] : ATTACH A COMMAND TO A KEY";
}

// Key map swapping command
string swapKeysCmd(const list<string> &args)
{
	if (args.size() >= 2)
	{
		std::list<string>::const_iterator arguments = args.begin();
		std::string const& keyNameA = *arguments++;
		int keyA = kName2Int(keyNameA);
		TEST_KEY(keyA, keyNameA);
		
		std::string const& keyNameB = *arguments++;
		int keyB = kName2Int(keyNameB);
		TEST_KEY(keyB, keyNameB);
		
		KeyHandler::swapKeyMapping(keyA, keyB);
		return "";
	}
	return "SWAPKEYS <KEY A> <KEY B> : SWAPS KEY A AND KEY B WITH EACHOTHER";
}

string setShiftChar(const list<string> &args)
{
	if (args.size() >= 2)
	{
		std::list<string>::const_iterator arguments = args.begin();
		
		std::string const& keyName = *arguments++;
		int key = kName2Int(keyName);
		TEST_KEY(key, keyName);
		
		int shiftCharacter = (*arguments)[0];
		
		KeyHandler::setShiftCharacter(key, shiftCharacter);
		return "";
	}
	return "SETSHIFTCHAR <KEY> <CHARACTER> : SETS THE CHARACTER TO BE USED WITH SHIFT+KEY";
}

string setChar(const list<string> &args)
{
	if (args.size() >= 2)
	{
		std::list<string>::const_iterator arguments = args.begin();
		
		std::string const& keyName = *arguments++;
		int key = kName2Int(keyName);
		TEST_KEY(key, keyName);
		
		int shiftCharacter = (*arguments)[0];
		
		KeyHandler::setCharacter(key, shiftCharacter);
		return "";
	}
	return "SETCHAR <KEY> <CHARACTER> : SETS THE CHARACTER TO BE USED WITH KEY";
}

string setConsoleKey(const list<string> &args)
{
	if (args.size() >= 1)
	{
		std::list<string>::const_iterator arguments = args.begin();
		
		std::string const& keyName = *arguments++;
		int key = kName2Int(keyName);
		TEST_KEY(key, keyName);

		//console.m_consoleKey = key; //TODO
		return "";
	}
	return "SETCONSOLEKEY <KEY> : SETS THE KEY TO SHOWS/HIDES THE CONSOLE";
}



string execCmd(const list<string> &args)
{
	if (!args.empty())
	{
		if ( console.executeConfig(*args.begin()) )
		{
			return "DONE";
		}
		return ( "COULDN'T EXEC " + *args.begin() );
	}
	return "EXEC <FILENAME> : EXECUTE A SCRIPT FILE";
}

string aliasCmd(const list<string> &args)
{
	string name;
	string action;
	
	if (!args.empty())
	{
		list<string>::const_iterator argument;
		argument=args.begin();
		
		name = *argument;
		
		argument++;
		
		if ( argument != args.end() )
		{
			action = *argument;
			console.registerAlias(name,action);
		}

		return "";
	}
	return "BIND <KEY> [COMMAND] : ATTACH A COMMAND TO A KEY";
}

/////////////////////////////// Console //////////////////////////////////////

//============================= LIFECYCLE ====================================

GConsole::GConsole()
: Console(256,39), m_consoleKey(KEY_F1), background(NULL)
{
	
}

//============================= INTERFACE ====================================

void GConsole::init()
{
	keyHandler.init();

	//m_mode = CONSOLE_MODE_BINDINGS;
	m_mode = CONSOLE_MODE_INPUT;

	console.registerVariables()
		("CON_SPEED", &speed, 4)
		("CON_HEIGHT", &height, 120)
	;
	
	console.registerCommands()
		("BIND", bindCmd)
		("SWAPKEYS", swapKeysCmd)
		("SETSHIFTCHAR", setShiftChar)
		("SETCHAR", setChar)
		("SETCONSOLEKEY", setConsoleKey)
		("EXEC", execCmd)
		("ALIAS", aliasCmd)
	;
}

void GConsole::shutDown()
{
	keyHandler.shutDown();
	
	//m_font must be deleted here!!!! hmm not sure now
}

void GConsole::loadResources()
{
	m_font = fontList.load("minifont.bmp");
	m_MaxMsgLength= (320-5) / m_font->width();	
	
	background = spriteList.load("con_background.bmp");
}

void GConsole::render(BITMAP* where, bool fullScreen)
{
	int textIndex = 0;
	list<std::string>::iterator msg = log.end();
	
	float pos = m_pos;
	if ( fullScreen ) pos = where->h-1;
		
	if ( pos > 0)
	{
		if (background) background->draw(where, 0, 0, static_cast<int>(pos), false, ALIGN_LEFT, ALIGN_BOTTOM);
		while ((msg != log.begin()) && ((int)pos - 20 - (textIndex - 1) * (m_font->height() + 1) > 0))
		{
			msg--;
			m_font->draw(where, *msg, 5, (int)pos - 20 - textIndex * (m_font->height() + 1), 0);
			textIndex++;
		}
		string tempString = (']' + m_inputBuff + '*');
		if ( tempString.length() < (where->w-5) / m_font->width() )
			m_font->draw(where, tempString, 5, (int)pos - 10 , 0);
		else
		{
			m_font->draw(where, tempString.substr(tempString.length() - (where->w-5) / m_font->width()), 5, (int)pos - 10 , 0);
		}
	}
}

void GConsole::checkInput()
{
	keyHandler.pollKeyboard();
	
	KeyEvent event = keyHandler.getEvent();

	while (event.type != KEY_EVENT_NONE) // While the event is not an end of list event
	{
		// Key Tilde is hardcoded to toogle the console (quake does the same so.. NO COMPLAINTS! :P)
		if ( (event.type == KEY_EVENT_PRESS) && (event.key == m_consoleKey) )
		{
			if ( m_mode == CONSOLE_MODE_INPUT )	// If the console is in input mode toogle to Binding mode
				m_mode = CONSOLE_MODE_BINDINGS;
			else											// If not toogle to input
			{
				m_mode = CONSOLE_MODE_INPUT;
				clear_keybuf();						// Clear allegro buffer so that old keys dont bother
				m_inputBuff.clear();
				currentCommand = commandsLog.end();
			}
		}else	// If the key was not Tilde continue to analize its bingings
		{
			if ( m_mode == CONSOLE_MODE_BINDINGS )		// Only if in bindings mode
			{
				if (event.type == KEY_EVENT_PRESS)
				{
					analizeKeyEvent(true, event.key);
				}else
				{
					analizeKeyEvent(false, event.key);
				}
			} else if ( m_mode == CONSOLE_MODE_INPUT )
			{
				if ( (event.type == KEY_EVENT_PRESS) && (event.key == KEY_UP) )
				{
					clear_keybuf();
					if (currentCommand != commandsLog.begin() )
						currentCommand--;
					if ( currentCommand == commandsLog.end() )
					{
						m_inputBuff.clear();
					}else
					{
						m_inputBuff = *currentCommand;
					}
				}
				if ( (event.type == KEY_EVENT_PRESS) && (event.key == KEY_DOWN) )
				{
					clear_keybuf();
					if (currentCommand != commandsLog.end() )
						currentCommand++;
					if ( currentCommand == commandsLog.end() )
					{
						m_inputBuff.clear();
					}else
					{
						m_inputBuff = *currentCommand;
					}
				}
			}
		}
		event = keyHandler.getEvent();	// Get next key event
	}
	
	if ( m_mode == CONSOLE_MODE_INPUT ) // console is in input read mode so..
	{
		if (keypressed())
		{
			char key;
			key=readkey();
			key=toupper(key);
			if (key == 8)//Backspace
			{
				if (!m_inputBuff.empty()) //if the string is not already empty...
					m_inputBuff.erase(m_inputBuff.length()-1); //delete last char
			}
			else if (key == 13) //Enter
			{
				addLogMsg(']'+m_inputBuff); //add the text to the console log
				console.parseLine(m_inputBuff); //parse the text
				commandsLog.push_back(m_inputBuff); //add the text to the commands log too
				currentCommand = commandsLog.end(); //reset the command log position
				m_inputBuff.clear(); // and then clear the buffer
			}
			else if (key == '\t') //Tab
			{
				string autoCompText = autoComplete( m_inputBuff );
				if (m_inputBuff == autoCompText)
				{
					listItems(m_inputBuff);
				}else
				{
					m_inputBuff = autoCompText;
				}
			}
			else // No special keys where detected so the char gets added to the string
			{
				m_inputBuff += key;
			}
		}
	}
}

void GConsole::think()
{
	if ( height > 240 ) height=240;
	if ( m_mode == CONSOLE_MODE_INPUT && m_pos < height )
	{
		m_pos+=speed;
	}else if ( m_mode == CONSOLE_MODE_BINDINGS && m_pos > 0 )
	{
		m_pos-=speed;
	}
	if (m_pos > height) m_pos = height;
	if (m_pos < 0) m_pos = 0;
}

//============================= PRIVATE ======================================


GConsole console;

