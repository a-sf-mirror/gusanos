#ifndef console_h
#define console_h

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
   ~Console(void);

	void registerIntVariable(const std::string &name, int* src, int defaultValue);
	void registerCommand(const std::string &name, std::string (*func)(const std::list<std::string>&));
	void parseLine(const std::string &text);
	void parse(std::list<std::string> &args);
	void bind(const std::string &key, const std::string &action);
	void setVariableValue(const std::string &name, const std::string &value);
	void addLogMsg(const std::string &msg);
	void analizeKeyEvent(bool state, char key);
	
	protected:
	
	BindTable bindTable;
	std::map<std::string, ConsoleItem*> items;
	std::list<std::string> log;
	int m_variableCount;
	int m_logMaxSize;
	int m_mode;
};

#endif  // _console_h_
