#ifndef LUA_CONTEXT_H
#define LUA_CONTEXT_H

extern "C"
{
#include "lua.h"
}

#include "types.h"
#include <iostream>
#include <vector>
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
	
	struct TableItemProxy
	{
		TableItemProxy(LuaContext& context)
		: m_context(context)
		{
		}
		
		template<class T>
		TableItemProxy const& operator()(char const* name, T v) const
		{
			lua_pushstring(m_context, name);
			m_context.push(v);
			lua_rawset(m_context, -3);
			return *this;
		}

		LuaContext& m_context;
	};
	
	struct CallProxy
	{
		CallProxy(LuaContext& context, LuaReference ref, int returns)
		: m_context(context), m_ref(ref), m_params(0), m_returns(returns)
		{
			m_context.push(errorReport);
			m_context.push(ref);
		}
		
		template<class T>
		CallProxy const& operator,(T const& p) const
		{
			++m_params;
			m_context.push(p);
			return *this;
		}
		
		int operator()() const
		{
			if(lua_isnil(m_context, -m_params-1))
				return 0;
			
			int r = m_context.call(m_params, m_returns, -m_params-2);
			
			if(r < 0)
			{
				m_context.pop(1);
				m_context.destroyReference(m_ref);
				return 0;
			}
			lua_remove(m_context, -r-1);
			return r;
		}

		LuaContext& m_context;
		LuaReference m_ref;
		mutable int m_params;
		int m_returns;
	};
	
	static int errorReport(lua_State* L);
	
	LuaContext();
	/*
	LuaContext(istream& stream);
	*/
	LuaContext(lua_State* state_);
	
	void init();
	
	void reset();
	
	static const char * istreamChunkReader(lua_State *L, void *data, size_t *size);

	void load(std::string const& chunk, istream& stream);
	/*
	void load(std::string const& chunk, istream& stream, string const& table);
	*/
	int call(int params = 0, int returns = 0, int errfunc = 0);
	
	LuaContext& push(lua_CFunction v)
	{
		lua_pushcfunction(m_State, v);
		return *this;
	}
	
	LuaContext& push(lua_Number v)
	{
		lua_pushnumber(m_State, v);
		return *this;
	}
	
	LuaContext& push(char const* v)
	{
		lua_pushstring(m_State, v);
		return *this;
	}
	
	LuaContext& push(std::string const& v)
	{
		push(v.c_str());
		return *this;
	}
	
	LuaContext& push(LuaReference v)
	{
		pushReference(v);
		return *this;
	}
	
	LuaContext& push(bool v)
	{
		lua_pushboolean(m_State, v);
		return *this;
	}
	
	LuaContext& push(int v)
	{
		lua_pushinteger(m_State, static_cast<lua_Integer>(v));
		return *this;
	}
			
	template<class T>
	LuaContext& push(T* v)
	{
		v->pushLuaReference();
		return *this;
	}
	
	template<class T>
	LuaContext& push(std::vector<T> const& v)
	{
		lua_newtable(m_State);
		for(size_t n = 0; n < v.size(); ++n)
		{
			push(v[n]);
			lua_rawseti(m_State, -2, n + 1);
		}
		return *this;
	}
	
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
	
	LuaContext& pushvalue(int i)
	{
		lua_pushvalue(m_State, i);
		return *this;
	}
	
	LuaContext& rawgeti(int table, int key)
	{
		lua_rawgeti(m_State, table, key);
		return *this;
	}
	
	LuaContext& rawseti(int table, int key)
	{
		lua_rawseti(m_State, table, key);
		return *this;
	}
	
	char const* tostring(int i)
	{
		return lua_tostring(m_State, i);
	}
	
	LuaContext& newtable()
	{
		lua_newtable(m_State);
		return *this;
	}
	
	LuaContext& pop(int c = 1)
	{
		lua_settop(m_State, (-1)-c);
		return *this;
	}
	
	CallProxy call(LuaReference ref, int returns = 0)
	{
		return CallProxy(*this, ref, returns);
	}
	
/*
	int callReference(LuaReference ref);
	
	template<class T1>
	int callReference(int ret, LuaReference ref, T1 const& p1)
	{
		pushReference(ref);
		push(p1);
		int result = call(1, ret);
		
		return result;
	}
	
	template<class T1, class T2>
	int callReference(int ret, LuaReference ref, T1 const& p1, T2 const& p2)
	{
		pushReference(ref);
		push(p1);
		push(p2);
		int result = call(2, ret);
		
		return result;
	}
	
	template<class T1, class T2, class T3>
	int callReference(int ret, LuaReference ref, T1 const& p1, T2 const& p2, T3 const& p3)
	{
		pushReference(ref);
		push(p1);
		push(p2);
		push(p3);
		int result = call(3, ret);
		
		return result;
	}
	
	template<class T1, class T2, class T3, class T4>
	int callReference(int ret, LuaReference ref, T1 const& p1, T2 const& p2, T3 const& p3, T4 const& p4)
	{
		pushReference(ref);
		push(p1);
		push(p2);
		push(p3);
		push(p4);
		int result = call(4, ret);
		
		return result;
	}
	
	template<class T1, class T2, class T3, class T4, class T5>
	int callReference(int ret, LuaReference ref, T1 const& p1, T2 const& p2, T3 const& p3, T4 const& p4, T5 const& p5)
	{
		pushReference(ref);
		push(p1);
		push(p2);
		push(p3);
		push(p4);
		push(p5);
		int result = call(5, ret);
		
		return result;
	}
	
	// Add more when needed...
*/
	template<class T>
	inline T get(int idx)
	{
		return T();
	}
	
	template<class T>
	void tableToVector(std::vector<T> const& v)
	{
		v.clear();
		for(size_t n = 1; ; ++n)
		{
			lua_rawgeti(m_State, -1, n + 1);
			if(lua_isnil(m_State, -1))
			{
				lua_settop(m_State, -3); // Pop nil and table
				return;
			}
			//TODO: v.push_back(get<T>(-1));
			lua_settop(m_State, -2); // Pop value
		}
	}
	
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
	
	TableItemProxy tableItems()
	{
		return TableItemProxy(*this);
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

template<>
inline bool LuaContext::get<bool>(int idx)
{
	return lua_toboolean(m_State, idx);
}

#endif //LUA_CONTEXT_H
