#ifndef XX_h
#define XX_h

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

#define VAR_TYPE_INVALID 0
#define VAR_TYPE_INT 1

using namespace std;

struct Variable
{

	Variable();
	Variable(int* src, int defaultValue, int type);
	
	int *m_value;
	int m_type;
	int m_defaultValue;
};

class Console
{
	public:

   Console(void);
   ~Console(void);

	void registerIntVariable(int* src, string name, int defaultValue);
	void registerCommand(string name, void (*func)(string arg));
	void parse(string text);
	void bind(char key, string action);
	void setVariableValue(string name, int value);
	
	private:
	
	KeyHandler keyHandler;
	BindTable bindTable;
	map<string, Variable> variables;
	int m_variableCount;
};

#endif  // _XX_h_
