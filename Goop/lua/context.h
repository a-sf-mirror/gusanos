#ifndef LUA_CONTEXT_H
#define LUA_CONTEXT_H

extern "C"
{
#include "lua.h"
}

#include "types.h"
#include <iostream>
#include <string>
using std::istream;
using std::cerr;
using std::endl;
using std::string;



class LuaContext
{
public:
	struct FunctionProxy
	{
		FunctionProxy(LuaContext& context)
		: m_context(context)
		{
		}
		
		FunctionProxy const& operator()(char const* name, lua_CFunction func) const
		{
			m_context.function(name, func);
			return *this;
		}

		LuaContext& m_context;
	};
	
	struct TableFunctionProxy
	{
		TableFunctionProxy(LuaContext& context)
		: m_context(context)
		{
		}
		
		TableFunctionProxy const& operator()(char const* name, lua_CFunction func) const
		{
			m_context.tableFunction(name, func);
			return *this;
		}

		LuaContext& m_context;
	};
	
	LuaContext();
	
	LuaContext(istream& stream);
	
	LuaContext(lua_State* state_);
	
	void init();
	
	void reset();
	
	static const char * istreamChunkReader(lua_State *L, void *data, size_t *size);

	void load(istream& stream);
	
	void load(istream& stream, string const& table);
	
	int call(int params = 0, int returns = 0);
	
	void push(lua_Number v);
	
	void push(const char* v);
	
	void push(LuaReference v);
	
	void push(bool v);
	
	void push(int v);
	
	template<class T>
	void pushFullReference(T& x, LuaReference metatable)
	{
		T** i = (T **)lua_newuserdata (m_State, sizeof(T *));
		*i = &x;
		pushReference(metatable);
		
		lua_setmetatable(m_State, -2);
	}
	
	template<class T>
	void pushFullReference(T& x)
	{
		T** i = (T **)lua_newuserdata (m_State, sizeof(T*));
		*i = &x;
	}
	
	template<class T>
	void pushObject(T const& x)
	{
		T* i = (T *)lua_newuserdata (m_State, sizeof(T));
		*i = x;
	}
	
	void* pushObject(LuaReference metatable, size_t count)
	{
		void* p = lua_newuserdata (m_State, count);
		
		pushReference(metatable);
		lua_setmetatable(m_State, -2);
		
		return p;
	}
	
	int callReference(LuaReference ref);
	
	template<class T1>
	int callReference(LuaReference ref, T1 const& p1)
	{
		pushReference(ref);
		push(p1);
		int result = call(1, 0);
		
		return result;
	}
	
	template<class T1, class T2>
	int callReference(LuaReference ref, T1 const& p1, T2 const& p2)
	{
		pushReference(ref);
		push(p1);
		push(p2);
		int result = call(2, 0);
		
		return result;
	}
	
	template<class T1, class T2, class T3>
	int callReference(LuaReference ref, T1 const& p1, T2 const& p2, T3 const& p3)
	{
		pushReference(ref);
		push(p1);
		push(p2);
		push(p3);
		int result = call(3, 0);
		
		return result;
	}
	
	template<class T1, class T2, class T3, class T4>
	int callReference(LuaReference ref, T1 const& p1, T2 const& p2, T3 const& p3, T4 const& p4)
	{
		pushReference(ref);
		push(p1);
		push(p2);
		push(p3);
		push(p4);
		int result = call(4, 0);
		
		return result;
	}
	
	template<class T1, class T2, class T3, class T4, class T5>
	int callReference(LuaReference ref, T1 const& p1, T2 const& p2, T3 const& p3, T4 const& p4, T5 const& p5)
	{
		pushReference(ref);
		push(p1);
		push(p2);
		push(p3);
		push(p4);
		push(p5);
		int result = call(5, 0);
		
		return result;
	}
	
	// Add more when needed...
	
	void function(char const* name, lua_CFunction func);
	
	void tableFunction(char const* name, lua_CFunction func);
	
	FunctionProxy functions()
	{
		return FunctionProxy(*this);
	}
	
	TableFunctionProxy tableFunctions()
	{
		return TableFunctionProxy(*this);
	}
	
	LuaReference createReference();
	
	void destroyReference(LuaReference ref);
	
	void pushReference(LuaReference ref);
	
	operator lua_State*()
	{
		return m_State;
	}
	
	~LuaContext();
	
private:
	lua_State *m_State;
	bool m_borrowed;
};


#endif //LUA_CONTEXT_H
