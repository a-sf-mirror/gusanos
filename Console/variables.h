#ifndef VARIABLES_H
#define VARIABLES_H

#include "consoleitem.h"
#include "text.h"
#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
#include <map>

#define VAR_TYPE_INVALID 0
#define VAR_TYPE_INT 1

class Variable : public ConsoleItem
{
	public:
	
	Variable(std::string const& name)
	: m_name(name)
	{
	}
	
	Variable()
	{
	}
	
	~Variable()
	{
	}

	std::string const& getName()
	{ return m_name; }
	
	protected:

	std::string m_name;
};

template<class T>
class TVariable : public Variable
{
	public:
	
	TVariable(std::string name, T* src, T defaultValue, void (*callback)( T ) = NULL )
	: Variable(name), m_src(src), m_callback(callback)
	{
		*src = defaultValue;
	}
	
	TVariable()
	: Variable(), m_src(NULL), m_defaultValue(T(0)), m_callback(NULL) //TODO: T(0) isn't always a good idea
	{
	}
	
	~TVariable()
	{
	}
	
	std::string invoke(const std::list<std::string> &args)
	{
		if (!args.empty())
		{
			T oldValue = *m_src;
			*m_src = cast<T>(*args.begin());
			if ( m_callback ) m_callback(oldValue);
	
			return "";
		}else
		{
			return m_name + " IS \"" + lexical_cast<std::string>(*m_src) + '"';
		}
	}
	
	private:

	void (*m_callback)( T );
	T* m_src;
	T m_defaultValue;
};

template<class T, class IT>
inline TVariable<T>* tVariable(std::string name, T* src, IT defaultValue, void (*callback)( T ) = NULL )
{
	return new TVariable<T>(name, src, defaultValue, callback);
}

/*
class IntVariable : public Variable
{
	public:
	
	IntVariable(int* src, std::string name, int defaultValue, void (*func)( int ) );
	IntVariable();
	~IntVariable();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:

	void (*callback)( int );
	int* m_src;
	int m_defaultValue;
};

class FloatVariable : public Variable
{
	public:
	
	FloatVariable( float* src, std::string name, float defaultValue, void (*func)( float ) );
	FloatVariable();
	~FloatVariable();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:
	
	void (*callback)( float );
	float* m_src;
	float m_defaultValue;
};*/

typedef TVariable<int> IntVariable;
typedef TVariable<float> FloatVariable;

class EnumVariable : public Variable
{
	public:
	
	typedef std::map<std::string, int, IStrCompare> MapType;
	typedef std::map<int, std::string> ReverseMapType;
	
	EnumVariable(std::string name, int* src, int defaultValue, MapType const& mapping, void (*func)( int ) = NULL);
	EnumVariable();
	~EnumVariable();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:
	
	void (*callback)( int );
	int* m_src;
	MapType m_mapping;
	ReverseMapType m_reverseMapping;
	int m_defaultValue;
};

#endif  // _VARIABLES_H_
