#include "bindings.h"

/////////////////////////////BindTable//////////////////////////////////

BindTable::BindTable()
{	
}

BindTable::~BindTable()
{
}

void BindTable::bind(char key, const std::string &action)
{
	binding[key].m_action = action;
}

std::string BindTable::getBindingAction(char key)
{
	return binding[key].m_action;
}

