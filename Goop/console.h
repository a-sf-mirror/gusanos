#ifndef console_h
#define console_h

// SYSTEM INCLUDES
//

// PROJECT INCLUDES
//
#include "keyboard.h"
#include "bindings.h"

// LOCAL INCLUDES
//
#include <string>
#include <list>
#include <map>

// FORWARD REFERENCES
//
class Variable;

using namespace std;

class Console
{
	public:

   Console(void);
   ~Console(void);

	void registerIntVariable(int* src, const string &name, int defaultValue);
	void registerCommand(const string &name, void (*func)(const string &arg));
	void parseLine(const string &text);
	void parse(const string &text);
	void bind(char key, const string &action);
	void setVariableValue(const string &name, const string &value);
	
	private:
	
	KeyHandler keyHandler;
	BindTable bindTable;
	map<string, Variable*> variables;
	int m_variableCount;
};

#endif  // _console_h_
