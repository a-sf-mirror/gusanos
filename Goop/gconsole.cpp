#include "gconsole.h"
#include "keyboard.h"
#include "keys.h"
#include "font.h"

#include <allegro.h>

using namespace std;

// Bind console command
string bindCmd(const list<string> &args)
{
	char key;
	string action;
	
	if (!args.empty())
	{
		list<string>::const_iterator argument;
		argument=args.begin();
		
		key = kName2Int(*argument);
		
		argument++;
		if ( argument != args.end() )
			console.bind(key, *argument);

		return "";
	}
	return "BIND <KEY> [COMMAND] : ATTACH A COMMAND TO A KEY";
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
	return "BIND <FILENAME> : EXECUTE A SCRIPT FILE";
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

GConsole::GConsole() : Console(256,39)
{
	background = NULL;
};

//============================= INTERFACE ====================================

void GConsole::init()
{
	keyHandler.init();

	m_font = fontList.load("minifont.bmp");
	
	m_MaxMsgLength= (320-5) / m_font->width();
	m_mode = CONSOLE_MODE_BINDINGS;
	
	background = spriteList.load("con_background.bmp");
	
	registerFloatVariable("CON_SPEED", &speed, 4);
	registerIntVariable("CON_HEIGHT", &height, 120);
	
	registerCommand("BIND", bindCmd);
	registerCommand("EXEC", execCmd);
	registerCommand("ALIAS", aliasCmd);
}

void GConsole::shutDown()
{
	keyHandler.shutDown();
	
	//m_font must be deleted here!!!! hmm not sure now
}

void GConsole::render(BITMAP* where)
{
	int textIndex = 0;
	list<std::string>::iterator msg = log.end();
	if ( m_pos > 0 )
	{
		background->draw(where, 0, 0, m_pos,0, false, ALIGN_LEFT, ALIGN_BOTTOM);
		while ((msg != log.begin()) && ((int)m_pos - 20 - (textIndex - 1) * (m_font->height() + 1) > 0))
		{
			msg--;
			m_font->draw(where, *msg, 5, (int)m_pos - 20 - textIndex * (m_font->height() + 1), 0);
			textIndex++;
		}
		string tempString = (']' + m_inputBuff + '*');
		if ( tempString.length() < (where->w-5) / m_font->width() )
			m_font->draw(where, tempString, 5, (int)m_pos - 10 , 0);
		else
		{
			m_font->draw(where, tempString.substr(tempString.length() - (where->w-5) / m_font->width()), 5, (int)m_pos - 10 , 0);
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
		if ( (event.type == KEY_EVENT_PRESS) && (event.key == KEY_TILDE) )
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

