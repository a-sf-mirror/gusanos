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
	LuaContext()
	: m_FirstFreeRef(1)
	{
		m_State = lua_open();
	}
	
	LuaContext(istream& stream)
	: m_FirstFreeRef(1)
	{
		m_State = lua_open();
		load(stream);
	}
	
	void reset()
	{
		if(m_State)
			lua_close(m_State);
		m_State = lua_open();
		m_FirstFreeRef = 1;
	}
	
	static const char * istreamChunkReader(lua_State *L, void *data, size_t *size)
	{
		static char buffer[1024];
		
		istream& stream = *(istream *)data;
		
		stream.read(buffer, 1024);

		*size = (size_t)stream.gcount();
		
		if(*size > 0)
			return buffer;
		else
			return 0;
	}

	void load(istream& stream)
	{
		int result = lua_load(m_State, istreamChunkReader, &stream, "chunk");
		
		if(result)
		{
			cerr << "Lua error: " << lua_tostring(m_State, -1) << endl;
			return;
		}
		
		//TODO: Create an error handler function that returns additional
		//debug info.
		result = lua_pcall (m_State, 0, 0, 0);
		
		switch(result)
		{
			case LUA_ERRRUN:
			case LUA_ERRMEM:
			case LUA_ERRERR:
			{
				/*strcpy(errMsgSpace, lua_tostring(m_State, -1));
				lua_pop(m_State, 1);*/
				//TODO: throw lua_exception(*this);
				cerr << "Lua error: " << lua_tostring(m_State, -1) << endl;
			}
			break;
		}

	}
	
	void load(istream& stream, string const& table)
	{
		int result = lua_load(m_State, istreamChunkReader, &stream, "chunk");
		
		if(result)
		{
			cerr << "Lua error: " << lua_tostring(m_State, -1) << endl;
			return;
		}
		
		lua_pushstring(m_State, table.c_str());
		lua_rawget(m_State, LUA_GLOBALSINDEX);
		lua_setfenv(m_State, -2);
		
		//TODO: Create an error handler function that returns additional
		//debug info.
		result = lua_pcall (m_State, 0, 0, 0);
		
		switch(result)
		{
			case LUA_ERRRUN:
			case LUA_ERRMEM:
			case LUA_ERRERR:
			{
				/*strcpy(errMsgSpace, lua_tostring(m_State, -1));
				lua_pop(m_State, 1);*/
				//TODO: throw lua_exception(*this);
				cerr << "Lua error: " << lua_tostring(m_State, -1) << endl;
			}
			break;
		}

	}
	
	int call(int params = 0, int returns = 0)
	{
		int result = lua_pcall (m_State, params, returns, 0);
			
		switch(result)
		{
			case LUA_ERRRUN:
			case LUA_ERRMEM:
			case LUA_ERRERR:
			{
				/*strcpy(errMsgSpace, lua_tostring(m_State, -1));
				lua_pop(m_State, 1);*/
				//TODO: throw lua_exception(*this);
				cerr << "Lua error: " << lua_tostring(m_State, -1) << endl;
				lua_settop(m_State, -1);
				return -1;
			}
			break;
		}
		
		return returns;
	}
	
	void push(double v)
	{
		lua_pushnumber(m_State, v);
	}
	
	void push(const char* v)
	{
		lua_pushstring(m_State, v);
	}
	
	int callReference(int ref)
	{
		pushReference(ref);
		int result = call(0, 0);
		
		return result;
	}
	
	template<class T1>
	int callReference(int ref, T1 const& p1)
	{
		pushReference(ref);
		push(p1);
		int result = call(1, 0);
		
		return result;
	}
	
	// Add more when needed...
	
	void function(char const* name, lua_CFunction func)
	{
		lua_pushstring(m_State, name);
		lua_pushcfunction(m_State, func);
		lua_settable(m_State, LUA_GLOBALSINDEX);
	}
	
	int createReference()
	{
		int ref = m_FirstFreeRef;
		//lua_pushvalue(m_State, idx);
		lua_rawseti(m_State, LUA_REGISTRYINDEX, ref);
		++m_FirstFreeRef;
		return ref;
	}
	
	void pushReference(int ref)
	{
		lua_rawgeti(m_State, LUA_REGISTRYINDEX, ref);
	}
	
	operator lua_State*()
	{
		return m_State;
	}
	
	~LuaContext()
	{
		if(m_State)
			lua_close(m_State);
	}
	
private:
	lua_State *m_State;
	int        m_FirstFreeRef;
};

#endif //LUA_CONTEXT_H
