#ifndef bindings_h
#define bindings_h

#include <string>

struct Binding
{
	std::string m_action;
};

class BindTable
{
	public:
	
	BindTable(void);
	~BindTable(void);
	
	void bind(char key, const std::string &action);
	void unBind(char key);
	void unBindAll();
	std::string getBindingAction(char key);
	
	private:
	
	Binding binding[256];
};

#endif  // _bindings_h_
