#ifndef bindings_h
#define bindings_h

#include <string>
#include <allegro.h>

using namespace std;

struct Binding
{
	string m_action;
};

class BindTable
{
	public:
	
	BindTable(void);
	~BindTable(void);
	
	void bind(char key, string action);
	void unBind(char key);
	void unBindAll();
	
	private:
	
	Binding binding[KEY_MAX];
};

#endif  // _bindings_h_
