#include "gconsole.h"
#include "keyboard.h"

#include <allegro.h>

using namespace std;

// Bind console command
string bindCmd(const list<string> &args)
{
	string key;
	string action;
	
	key = *args.begin();
	action = *(++args.begin());
	console.bind(key, action);
	return "";
}

/////////////////////////////// Console //////////////////////////////////////

//============================= LIFECYCLE ====================================

//============================= INTERFACE ====================================

void GConsole::init()
{
	keyHandler.init();
	
	m_mode = CONSOLE_MODE_BINDINGS;
	
	registerCommand("BIND", bindCmd);
}

void GConsole::shutDown()
{
	keyHandler.shutDown();
}

// Temporal implementation (I need bitmap fonts :P)
void GConsole::render(BITMAP* where)
{
	int textIndex = 0;
	list<std::string>::iterator msg = log.end();
	while ((msg != log.begin()) && (textIndex < 20))
	{
		msg--;
		textout_ex(where, font, (*msg).c_str(), 5, where->h - 20 - textIndex * 10, makecol(255,255,255), -1);
		textIndex++;
	}
	string tempString = (']' + m_inputBuff + '*');
	if ( tempString.length() < 39 )
		textout_ex(where, font, tempString.c_str(), 5, where->h - 10, makecol(255,255,255), -1);
	else
	{
		textout_ex(where, font, tempString.substr(tempString.length() - 39).c_str(), 5, where->h - 10, makecol(255,255,255), -1);
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
				m_inputBuff.clear(); // and then clear the buffer
			}
			else // No special keys where detected so the char gets added to the string
			{
				m_inputBuff += key;
			}
		}
	}
}




//============================= PRIVATE ======================================


GConsole console;

