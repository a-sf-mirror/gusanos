#ifndef bindings_h
#define bindings_h

#include <string>
#include <allegro.h>

struct Binding
{
	std::string m_action;
};

class BindTable
{
	public:
	
	BindTable(void);
	~BindTable(void);
	
	void bind(char key, std::string action);
	void unBind(char key);
	void unBindAll();
	
	private:
	
	Binding binding[KEY_MAX];
};

#endif  // _bindings_h_
