#ifndef VARIABLES_H
#define VARIABLES_H

#include "consoleitem.h"
#include "text.h"
//#include <boost/lexical_cast.hpp>
//using boost::lexical_cast;

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
	
	virtual ~Variable()
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
	: Variable(), m_src(NULL), m_defaultValue(T()), m_callback(NULL)
	{
	}
	
	virtual ~TVariable()
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
			//return m_name + " IS \"" + cast<std::string>(*m_src) + '"';
			return cast<std::string>(*m_src);
		}
	}
	
	private:

	T* m_src;
	T m_defaultValue;
	void (*m_callback)( T );
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
typedef TVariable<std::string> StringVariable;

class EnumVariable : public Variable
{
	public:
	
	typedef std::map<std::string, int, IStrCompare> MapType;
	typedef std::map<int, std::string> ReverseMapType;
	
	EnumVariable(std::string name, int* src, int defaultValue, MapType const& mapping, void (*func)( int ) = NULL);
	EnumVariable();
	virtual ~EnumVariable();
	
	std::string invoke(const std::list<std::string> &args);
	
	private:
	
	void (*callback)( int );
	int* m_src;
	int m_defaultValue;	
	ReverseMapType m_reverseMapping;
	MapType m_mapping;
};

#endif  // _VARIABLES_H_
