#ifndef VARIABLES_H
#define VARIABLES_H

#include "consoleitem.h"

#include <string>

#define VAR_TYPE_INVALID 0
#define VAR_TYPE_INT 1

class IntVariable : public ConsoleItem
{
	public:
	
	IntVariable(int* src, std::string name, int defaultValue);
	IntVariable();
	~IntVariable();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:
	
	std::string m_name;
	int* m_src;
	int m_defaultValue;
};

class FloatVariable : public ConsoleItem
{
	public:
	
	FloatVariable( float* src, std::string name, float defaultValue);
	FloatVariable();
	~FloatVariable();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:
	
	std::string m_name;
	float* m_src;
	float m_defaultValue;
};

#endif  // _VARIABLES_H_
