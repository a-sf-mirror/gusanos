#ifndef variables_h
#define variables_h

#include <string>
#include <allegro.h>

#define VAR_TYPE_INVALID 0
#define VAR_TYPE_INT 1

class Variable
{
	public:
		
	virtual void setValue(std::string value) = 0;
	virtual std::string getValue() = 0;
	int getType();
	
	private:
	
	int m_type;
};

class IntVariable : public Variable
{
	public:
	
	IntVariable(int* src, int defaultValue);
	IntVariable();
	~IntVariable();
	
	void setValue(std::string value);
	std::string getValue();
	
	private:
	
	int* m_src;
	int m_defaultValue;
	int m_type;
};

#endif  // _variables_h_
