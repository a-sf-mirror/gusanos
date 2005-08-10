#ifndef LUA_CONTEXT_H
#define LUA_CONTEXT_H

extern "C"
{
#include "lua.h"
}

#include <iostream>
#include <string>
using std::istream;
using std::cerr;
using std::endl;
using std::string;

class LuaContext
{
public:
	LuaContext();
	
	LuaContext(istream& stream);
	
	void reset();
	
	static const char * istreamChunkReader(lua_State *L, void *data, size_t *size);

	void load(istream& stream);
	
	void load(istream& stream, string const& table);
	
	int call(int params = 0, int returns = 0);
	
	void push(double v);
	
	void push(const char* v);
	
	int callReference(int ref);
	
	template<class T1>
	int callReference(int ref, T1 const& p1)
	{
		pushReference(ref);
		push(p1);
		int result = call(1, 0);
		
		return result;
	}
	
	// Add more when needed...
	
	void function(char const* name, lua_CFunction func);
	
	int createReference();
	
	void pushReference(int ref);
	
	operator lua_State*()
	{
		return m_State;
	}
	
	~LuaContext();
	
private:
	lua_State *m_State;
	int        m_FirstFreeRef;
};

#endif //LUA_CONTEXT_H
