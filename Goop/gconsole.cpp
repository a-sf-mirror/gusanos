#include "gconsole.h"
#include "keyboard.h"
#include "keys.h"
#include "font.h"

#include <allegro.h>
#include <boost/bind.hpp>

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

string echoCmd(list<string> const& args)
{
	if(args.size() > 0)
	{
		std::list<string>::const_iterator i = args.begin();
		std::string ret = *i++;
		
		for(; i != args.end(); ++i)
		{
			ret += ' ' + *i;
		}
		
		return ret;
	}
	
	return "ECHO <ARGS> ... : PRINTS OUT ARGS TO THE CONSOLE";
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

string setAltGrChar(const list<string> &args)
{
	if (args.size() >= 2)
	{
		std::list<string>::const_iterator arguments = args.begin();
		
		std::string const& keyName = *arguments++;
		int key = kName2Int(keyName);
		TEST_KEY(key, keyName);
		
		int altgrCharacter = (*arguments)[0];
		
		KeyHandler::setAltGrCharacter(key, altgrCharacter);
		return "";
	}
	return "SETALTGRCHAR <KEY> <CHARACTER> : SETS THE CHARACTER TO BE USED WITH ALTGR+KEY";
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
: Console(256), m_consoleKey(KEY_TILDE), background(NULL)
{
	
}

//============================= INTERFACE ====================================

void GConsole::varCbFont( std::string oldValue )
{
	Font* newFont = fontLocator.load(console.m_fontName);
	if(!newFont)
	{
		console.addLogMsg("FONT \"" + console.m_fontName + "\" NOT FOUND, REVERTING TO OLD FONT");
		console.m_fontName = oldValue;
		return;
	}
	console.m_font = newFont;
}

void GConsole::init()
{
	//keyHandler.init();
	
	//Connect the handlers as group 0 so they are called first
	keyHandler.printableChar.connect(0, boost::bind(&GConsole::eventPrintableChar, this, _1));
	keyHandler.keyDown.connect(0, boost::bind(&GConsole::eventKeyDown, this, _1));
	keyHandler.keyUp.connect(0, boost::bind(&GConsole::eventKeyUp, this, _1));

	m_mode = CONSOLE_MODE_BINDINGS;
	//m_mode = CONSOLE_MODE_INPUT;

	console.registerVariables()
		("CON_SPEED", &speed, 4)
		("CON_HEIGHT", &height, 120)
		("CON_FONT", &m_fontName, "minifont", varCbFont)
	;

	console.registerCommands()
		("BIND", bindCmd)
		("SWAPKEYS", swapKeysCmd)
		("SETSHIFTCHAR", setShiftChar)
		("SETALTGRCHAR", setAltGrChar)
		("SETCHAR", setChar)
		("SETCONSOLEKEY", setConsoleKey)
		("EXEC", execCmd)
		("ALIAS", aliasCmd)
		("ECHO", echoCmd)
	;
	
	commandsLog.push_back("MOO");
	currentCommand = commandsLog.end(); //To workaround a crashbug with uninitialized iterator
}

void GConsole::shutDown()
{
	keyHandler.shutDown();
	
	//m_font must be deleted here!!!! hmm not sure now
}

void GConsole::loadResources()
{
	m_font = fontLocator.load(m_fontName);

	if(!m_font)
		cout << "Console font couldn't be loaded" << endl;
	
	background = spriteList.load("con_background.bmp");
}

void GConsole::render(BITMAP* where, bool fullScreen)
{
	//int textIndex = 0;

	float pos = m_pos;
	if ( fullScreen ) pos = where->h-1;
		
	if ( pos > 0)
	{
		if (background) background->draw(where, 0, 0, static_cast<int>(pos), false, ALIGN_LEFT, ALIGN_BOTTOM);

		int y = static_cast<int>(pos) - 5;
		
		string tempString = (']' + m_inputBuff + '*');

		std::pair<int, int> dim;
		string::const_reverse_iterator b = tempString.rbegin(), e = tempString.rend();
		// When using reverse iterators, fitString tries to fit the spacing of
		// the last character as well which isn't exactly what is wanted
		e = m_font->fitString(b, e, 320-5, dim);
		y -= dim.second;
		m_font->draw(where, e.base(), b.base(), 5, y);
				
		for(list<string>::reverse_iterator msgiter = log.rbegin();
		    msgiter != log.rend() && y > 0;
		    ++msgiter)
		{
			string const& msg = *msgiter;
			
			string::const_iterator b = msg.begin(), e = msg.end(), n;
			
			int totalHeight = 0;
			do
			{
				pair<int, int> dim;
				n = m_font->fitString(b, e, 320-5, dim);
				if(n == b)
					break;
				b = n;
				totalHeight += dim.second;
			}
			while(b != e);
			
			y -= totalHeight + 1;
			
			b = msg.begin();
			
			int y2 = y;
			
			do
			{
				pair<int, int> dim;
				n = m_font->fitString(b, e, 320-5, dim);
				if(n == b)
					break;
				m_font->draw(where, b, n, 5, y2, 0);
				y2 += dim.second;
				
				b = n;
			}
			while(b != e);
		}
	}
}

void GConsole::checkInput()
{
	keyHandler.pollKeyboard();
	
	/*
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
	}*/
}

bool GConsole::eventKeyDown(int k)
{
	if(k == m_consoleKey)
	{
		if ( m_mode == CONSOLE_MODE_INPUT )	// If the console is in input mode toogle to Binding mode
		{
			m_mode = CONSOLE_MODE_BINDINGS;
			return false;
		}
		else											// If not toogle to input
		{
			m_mode = CONSOLE_MODE_INPUT;
			clear_keybuf();						// Clear allegro buffer so that old keys dont bother
			m_inputBuff.clear();
			currentCommand = commandsLog.end();
		}
	}
	else if ( m_mode == CONSOLE_MODE_BINDINGS )		// Only if in bindings mode
	{
		analizeKeyEvent(true, k);
	}
	else if ( m_mode == CONSOLE_MODE_INPUT )
	{
		if ( k == KEY_UP )
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
		else if ( k == KEY_DOWN )
		{
			clear_keybuf();
			if (currentCommand != commandsLog.end() )
				currentCommand++;
			if ( currentCommand == commandsLog.end() )
			{
				m_inputBuff.clear();
			}
			else
			{
				m_inputBuff = *currentCommand;
			}
		}
		
		return false;
	}
	
	return true;
}

bool GConsole::eventKeyUp(int k)
{
	if ( m_mode == CONSOLE_MODE_BINDINGS )		// Only if in bindings mode
	{
		analizeKeyEvent(false, k);
	}
	else
		return false;
	
	return true;
}

bool GConsole::eventPrintableChar(char c)
{

	if ( m_mode == CONSOLE_MODE_INPUT ) // console is in input read mode so..
	{
		if (c == 8)//Backspace
		{
			if (!m_inputBuff.empty()) //if the string is not already empty...
				m_inputBuff.erase(m_inputBuff.length()-1); //delete last char
		}
		else if (c == 13) //Enter
		{
			addLogMsg(']'+m_inputBuff); //add the text to the console log
			console.parseLine(m_inputBuff); //parse the text
			commandsLog.push_back(m_inputBuff); //add the text to the commands log too
			currentCommand = commandsLog.end(); //reset the command log position
			m_inputBuff.clear(); // and then clear the buffer
		}
		else if (c == '\t') //Tab
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
			//m_inputBuff += toupper(c);
			m_inputBuff += c;
		}
		return false;
	}
	return true;
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

