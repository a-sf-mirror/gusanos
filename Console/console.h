#ifndef CONSOLE_H
#define CONSOLE_H

// SYSTEM INCLUDES
//

// PROJECT INCLUDES
//
#include "bindings.h"

// LOCAL INCLUDES
//
#include <string>
#include <list>
#include <map>

// FORWARD REFERENCES
//
class ConsoleItem;

class Console
{
	public:

   Console(void);
	Console(int logMaxSize, int MaxMsgLength);
   ~Console(void);

	void registerIntVariable(const std::string &name, int* src, int defaultValue);
	void registerFloatVariable(const std::string &name, float* src, float defaultValue);
	void registerAlias(const std::string &name, const std::string &action);
	void registerCommand(const std::string &name, std::string (*func)(const std::list<std::string>&));
	void parseLine(const std::string &text, bool parseRelease = false);
	void parse(std::list<std::string> &args, bool parseRelease);
	void bind(char key, const std::string &action);
	void addLogMsg(const std::string &msg);
	void analizeKeyEvent(bool state, char key);
	int executeConfig(const std::string &filename);
	std::string autoComplete(const std::string &text);
	void listItems(const std::string &text);
	
	protected:
	
	BindTable bindTable;
	std::map<std::string, ConsoleItem*> items;
	std::list<std::string> log;
	int m_variableCount;
	unsigned int m_logMaxSize;
	unsigned int m_MaxMsgLength;
	int m_mode;
};

#endif  // _CONSOLE_H_
