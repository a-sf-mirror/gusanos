#ifndef VARIABLES_H
#define VARIABLES_H

#include "consoleitem.h"

#include <string>
#include <map>

#define VAR_TYPE_INVALID 0
#define VAR_TYPE_INT 1

class IntVariable : public ConsoleItem
{
	public:
	
	IntVariable(int* src, std::string name, int defaultValue, void (*func)( int ) );
	IntVariable();
	~IntVariable();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:

	void (*callback)( int );
	std::string m_name;
	int* m_src;
	int m_defaultValue;
};

class FloatVariable : public ConsoleItem
{
	public:
	
	FloatVariable( float* src, std::string name, float defaultValue, void (*func)( float ) );
	FloatVariable();
	~FloatVariable();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:
	
	void (*callback)( float );
	std::string m_name;
	float* m_src;
	float m_defaultValue;
};

class EnumVariable : public ConsoleItem
{
	public:
	
	typedef std::map<std::string, int> MapType;
	typedef std::map<int, std::string> ReverseMapType;
	
	EnumVariable(int* src, std::string name, int defaultValue, MapType const& mapping, void (*func)( int ));
	EnumVariable();
	~EnumVariable();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:
	
	void (*callback)( int );
	std::string m_name;
	int* m_src;
	MapType m_mapping;
	ReverseMapType m_reverseMapping;
	int m_defaultValue;
};

#endif  // _VARIABLES_H_
