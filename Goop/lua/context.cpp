#include "context.h"

extern "C"
{
	#include "lualib.h"
}

#include <iostream>
using std::cerr;
using std::endl;

#define FREELIST_REF 1
#define ARRAY_SIZE   2

int LuaContext::errorReport(lua_State* L)
{
	LuaContext context(L);
	
	char const* s = lua_tostring(context, 1);
	
	lua_Debug info;
	
	cerr << "Lua error: " << s << ", call stack:" << endl;
	for(int i = 1; lua_getstack(context, i, &info); ++i)
	{
		lua_getinfo (context, "Snl", &info);
		
		char const* name = info.name ? info.name : "N/A";
		
		cerr << i << ". " << name << " (" << info.namewhat << ") @ " << info.short_src << ":" << info.currentline << endl;
	}
	
	context.pushvalue(1);
	return 1;
}

LuaContext::LuaContext()
: m_borrowed(false)
{
	init();
}

/*
LuaContext::LuaContext(istream& stream)
: m_borrowed(false)
{
	init();
	load(stream);
}
*/
LuaContext::LuaContext(lua_State* state_)
: m_borrowed(true), m_State(state_)
{

}

namespace
{
	void* l_alloc (void*, void* ptr, size_t, size_t nsize)
	{
		if (nsize == 0)
		{
			free(ptr);
			return 0;
		}
		else
			return realloc(ptr, nsize);
	}
}

void LuaContext::init()
{
	//m_State = lua_open();
	m_State = lua_newstate(l_alloc, 0);
	lua_pushnumber(m_State, 3);
	lua_rawseti(m_State, LUA_REGISTRYINDEX, ARRAY_SIZE);
	
	luaopen_base(m_State);
	luaopen_table(m_State);
	luaopen_string(m_State);
	luaopen_math(m_State);
}

void LuaContext::reset()
{
	if(m_borrowed)
		return;
		
	if(m_State)
		lua_close(m_State);
	init();
}

const char * LuaContext::istreamChunkReader(lua_State *L, void *data, size_t *size)
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

void LuaContext::load(std::string const& chunk, istream& stream)
{
	lua_pushcfunction(m_State, errorReport);
	int result = lua_load(m_State, istreamChunkReader, &stream, chunk.c_str());
	
	if(result)
	{
		cerr << "Lua error: " << lua_tostring(m_State, -1) << endl;
		pop(1);
		return;
	}
	
	//TODO: Create an error handler function that returns additional
	//debug info.
	result = lua_pcall (m_State, 0, 0, -2);
	
	switch(result)
	{
		case LUA_ERRRUN:
		case LUA_ERRMEM:
		case LUA_ERRERR:
		{
			/*strcpy(errMsgSpace, lua_tostring(m_State, -1));
			lua_pop(m_State, 1);*/
			//TODO: throw lua_exception(*this);
			//cerr << "Lua error: " << lua_tostring(m_State, -1) << endl;
			pop(1);
		}
		break;
	}
	pop(1); // Pop error function
}

/*
void LuaContext::load(std::string const& chunk, istream& stream, string const& table)
{
	lua_pushcfunction(m_State, errorReport);
	int result = lua_load(m_State, istreamChunkReader, &stream, chunk.c_str());
	
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
			//TODO: throw lua_exception(*this);
			cerr << "Lua error: " << lua_tostring(m_State, -1) << endl;
		}
		break;
	}
}*/

int LuaContext::call(int params, int returns, int errfunc)
{
	int result = lua_pcall (m_State, params, returns, errfunc);
		
	switch(result)
	{
		case LUA_ERRRUN:
		case LUA_ERRMEM:
		case LUA_ERRERR:
		{
			pop(1);
			return -1;
		}
		break;
	}
	
	return returns;
}

/*
int LuaContext::callReference(LuaReference ref)
{
	pushReference(ref);
	int result = call(0, 0);
	
	return result;
}*/

void LuaContext::function(char const* name, lua_CFunction func)
{
	lua_pushstring(m_State, name);
	lua_pushcfunction(m_State, func);
	lua_settable(m_State, LUA_GLOBALSINDEX);
}

void LuaContext::tableFunction(char const* name, lua_CFunction func)
{
	lua_pushstring(m_State, name);
	lua_pushcfunction(m_State, func);
	lua_rawset(m_State, -3);
}

LuaReference LuaContext::createReference()
{
	int ref;
	int t = LUA_REGISTRYINDEX;
	lua_rawgeti(m_State, t, FREELIST_REF);
	//ref = (int)lua_tonumber(m_State, -1);
	ref = static_cast<int>(lua_tointeger(m_State, -1));
	lua_settop(m_State, -2);
	if(ref != 0)
	{
		lua_rawgeti(m_State, t, ref);
		lua_rawseti(m_State, t, FREELIST_REF);
	}
	else
	{
		lua_rawgeti(m_State, t, ARRAY_SIZE);
		//ref = (int)lua_tonumber(m_State, -1);
		ref = static_cast<int>(lua_tointeger(m_State, -1));
		//lua_pushnumber(m_State, ref + 1);
		lua_pushinteger(m_State, static_cast<lua_Integer>(ref + 1));
		lua_rawseti(m_State, t, ARRAY_SIZE);
		lua_settop(m_State, -2);
	}
	
	lua_rawseti(m_State, t, ref);
	return LuaReference(ref);
}

void LuaContext::destroyReference(LuaReference ref)
{
	if (ref.idx > 2)
	{
    	int t = LUA_REGISTRYINDEX;
		lua_rawgeti(m_State, t, FREELIST_REF);
		lua_rawseti(m_State, t, ref.idx);  /* t[ref] = t[FREELIST_REF] */
		//lua_pushnumber(m_State, (lua_Number)ref.idx);
		lua_pushinteger(m_State, static_cast<lua_Integer>(ref.idx));
		lua_rawseti(m_State, t, FREELIST_REF);  /* t[FREELIST_REF] = ref */
	}
}

void LuaContext::pushReference(LuaReference ref)
{
	lua_rawgeti(m_State, LUA_REGISTRYINDEX, ref.idx);
}


LuaContext::~LuaContext()
{
	if(m_State && !m_borrowed)
		lua_close(m_State);
}
