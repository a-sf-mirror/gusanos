#ifndef CONSOLE_H
#define CONSOLE_H

// SYSTEM INCLUDES
//

// PROJECT INCLUDES
//
#include "bindings.h"
#include "text.h"
#include "variables.h"

// LOCAL INCLUDES
//
#include <string>
#include <list>
#include <map>

// FORWARD REFERENCES
//
class ConsoleItem;

class Console;

struct RegisterVariableProxy
{
	RegisterVariableProxy(Console& console)
	: m_console(console)
	{
	}
	
	template<class T, class IT>
	RegisterVariableProxy const& operator()(std::string name, T* src, IT defaultValue, void (*callback)( T ) = NULL ) const
	{
		m_console.registerVariable(new TVariable<T>(name, src, defaultValue, callback));
		return *this;
	}
	
	template<class VarT>
	RegisterVariableProxy const& operator()(VarT* var) const
	{
		m_console.registerVariable(var);
		return *this;
	}
	
	Console& m_console;
};

struct RegisterCommandProxy
{
	RegisterCommandProxy(Console& console)
	: m_console(console)
	{
	}
	
	template<class FT>
	RegisterCommandProxy const& operator()(std::string name, FT func) const
	{
		m_console.registerCommand(name, func);
		return *this;
	}
	
	Console& m_console;
};

class Console
{
	public:

	Console(void);
	Console(int logMaxSize, int MaxMsgLength);
	~Console(void);

/*
	void registerIntVariable(const std::string &name, int* src, int defaultValue, void (*func)( int ) = NULL);
	void registerFloatVariable(const std::string &name, float* src, float defaultValue, void (*func)( float ) = NULL);
	void registerEnumVariable(std::string const& name, int* src, int defaultValue, std::map<std::string, int, IStrCompare> const& mapping, void (*func)( int ) = NULL);
*/
	void registerVariable(Variable* var);
	RegisterVariableProxy registerVariables()
	{
		return RegisterVariableProxy(*this);
	}
	
	RegisterCommandProxy registerCommands()
	{
		return RegisterCommandProxy(*this);
	}
	
	void registerAlias(const std::string &name, const std::string &action);
	void registerCommand(const std::string &name, std::string (*func)(const std::list<std::string>&));
	void registerSpecialCommand(const std::string &name, int index, std::string (*func)(int,const std::list<std::string>&));
	
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
	std::map<std::string, ConsoleItem*, IStrCompare> items;
	std::list<std::string> log;
	int m_variableCount;
	unsigned int m_logMaxSize;
	unsigned int m_MaxMsgLength;
	int m_mode;
};

#endif  // _CONSOLE_H_
