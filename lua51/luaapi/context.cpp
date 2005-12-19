#include "context.h"

extern "C"
{
	#include "../lualib.h"
}

#include <iostream>
using std::cerr;
using std::endl;

#include <zoidcom.h>
#include <cmath>

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

LuaContext::LuaContext(LuaContext const& b)
: m_borrowed(true), m_State(b.m_State)
{
}

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
	lua_pushinteger(m_State, 3);
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
			//cerr << "Lua error: " << lua_tostring(m_State, -1) << endl;
			pop(1);
			return -1;
		}
		break;
	}
	
	return result;
}

/*
int LuaContext::callReference(LuaReference ref)
{
	pushReference(ref);
	int result = call(0, 0);
	
	return result;
}*/

void LuaContext::function(char const* name, lua_CFunction func, int upvalues)
{
	lua_pushcclosure(m_State, func, upvalues);
	lua_setfield(m_State, LUA_GLOBALSINDEX, name);
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

namespace LuaType
{
enum type
{
	Nil = 0,
	Number,
	BooleanFalse,
	BooleanTrue,
	String,
	Table,
	UserData,
	End,
	Integer,
};
}

void LuaContext::serialize(ZCom_BitStream& s, int i)
{
	switch(lua_type(m_State, i))
	{
		case LUA_TNIL:
			s.addInt(LuaType::Nil, 4);
		break;
		
		case LUA_TNUMBER:
		{
			lua_Number n = lua_tonumber(m_State, i);
			lua_Integer i = static_cast<lua_Integer>(n);
			if(std::fabs(i - n) < 0.00001)
			{
				s.addInt(LuaType::Integer, 4);
				s.addInt(i, 32);
			}
			else
			{
				s.addInt(LuaType::Number, 4);
				s.addFloat(static_cast<float>(n), 23);
			}
		}
		break;
		
		case LUA_TBOOLEAN:
		{
			int n = lua_toboolean(m_State, i);
			s.addInt(n ? LuaType::BooleanTrue : LuaType::BooleanFalse, 4);
		}
		break;
		
		case LUA_TSTRING:
		{
			s.addInt(LuaType::String, 4);
			char const* n = lua_tostring(m_State, i);
			s.addString(n);
		}
		break;
				
		case LUA_TTABLE:
		{
			s.addInt(LuaType::Table, 4);
			size_t idx = 1;
			for(;; ++idx)
			{
				lua_rawgeti(m_State, i, idx);
				if(lua_isnil(m_State, -1))
				{
					pop(1);
					break;
				}

				serialize(s, -1);
				pop(1);
			}
			s.addInt(LuaType::End, 4);
			
			lua_pushnil(m_State);
			int tab = i < 0 ? i - 1 : i;
			while(lua_next(m_State, tab) != 0)
			{
				if(!lua_isnumber(m_State, -2) || lua_tointeger(m_State, -2) >= idx)
				{
					serialize(s, -2);
					serialize(s, -1);
				}
				pop(1);
			}
			s.addInt(LuaType::End, 4);
		}
		break;
		
		default: // Ignore any value we can't handle and encode a nil instead
			s.addInt(LuaType::Nil, 4);
		break;
	}
}

bool LuaContext::deserialize(ZCom_BitStream& s)
{
	int t = s.getInt(4);
	switch(t)
	{
		case LuaType::Nil:
			lua_pushnil(m_State);
		break;
		
		case LuaType::Number:
		{
			push(static_cast<lua_Number>(s.getFloat(23)));
		}
		break;
		
		case LuaType::Integer:
		{
			push(s.getInt(32));
		}
		break;
		
		case LuaType::BooleanTrue:
			push(true);
		break;
		
		case LuaType::BooleanFalse:
			push(false);
		break;
		
		case LuaType::String:
		{
			push(s.getStringStatic());				
		}
		break;
		
		case LuaType::Table:
		{
			lua_newtable(m_State);
			
			for(int idx = 1; deserialize(s); ++idx)
			{
				lua_rawseti(m_State, -2, idx);
			}
			
			while(deserialize(s))
			{
				if(!deserialize(s))
				{
					// Value was invalid
					pop(1); // Pop key
					return true; // Return what we have of the table
				}
				
				lua_rawset(m_State, -3);
			}
		}
		break;
		
		default:
		{
			return false; // No value could be decoded
		}
		break;
	}
	
	return true;
}

void LuaContext::serialize(std::ostream& s, int i)
{
	switch(lua_type(m_State, i))
	{
		case LUA_TNIL:
			s.put(LuaType::Nil);
		break;
		
		case LUA_TNUMBER:
		{
			lua_Number n = lua_tonumber(m_State, i);
			s.put(LuaType::Number);
			s.write((char *)&n, sizeof(double));
		}
		break;
		
		case LUA_TBOOLEAN:
		{
			int n = lua_toboolean(m_State, i);
			s.put(n ? LuaType::BooleanTrue : LuaType::BooleanFalse);
		}
		break;
		
		case LUA_TSTRING:
		{
			s.put(LuaType::String);
			size_t len;
			char const* n = lua_tolstring(m_State, i, &len);
			s.put((len >> 24) & 0xFF);
			s.put((len >> 16) & 0xFF);
			s.put((len >> 8) & 0xFF);
			s.put(len & 0xFF);
			s.write(n, len);
		}
		break;
		
		case LUA_TTABLE:
		{
			s.put(LuaType::Table);
			lua_pushnil(m_State);
			int tab = i < 0 ? i - 1 : i;
			while(lua_next(m_State, tab) != 0)
			{
				serialize(s, -2);
				serialize(s, -1);
				pop(1);
			}
			s.put(LuaType::End);
		}
		break;
		
		default: // Ignore any value we can't handle and encode a nil instead
			s.put(LuaType::Nil);
		break;
	}
}

void LuaContext::deserialize(std::istream& s)
{
	int t = s.get();
	switch(t)
	{
		case LuaType::Nil:
			lua_pushnil(m_State);
		break;
		
		case LuaType::Number:
		{
			lua_Number n;
			s.read((char *)&n, sizeof(double));
			push(n);
		}
		break;
		
		case LuaType::BooleanTrue:
			push(true);
		break;
		
		case LuaType::BooleanFalse:
			push(false);
		break;
		
		case LuaType::String:
		{
			int v1 = s.get();
			int v2 = s.get();
			int v3 = s.get();
			int v4 = s.get();
			size_t len = (v1 << 24) | (v2 << 16) | (v3 << 8) | v4;
			char* str = new char[len];
			try
			{
				s.read(str, len);
				lua_pushlstring(m_State, str, len);				
			}
			catch(...)
			{
				delete[] str;
				throw;
			}
			delete[] str;
		}
		break;
		
		case LuaType::Table:
		{
			lua_newtable(m_State);
			while(s && s.peek() != LuaType::End)
			{
				deserialize(s);
				deserialize(s);
				lua_rawset(m_State, -3);
			}
			s.get(); // Skip end
		}
		break;
	}
}

void LuaContext::serializeT(std::ostream& s, int i, int indent)
{
	switch(lua_type(m_State, i))
	{
		case LUA_TNIL:
			s << "nil";
		break;
		
		case LUA_TNUMBER:
		{
			lua_Number n = lua_tonumber(m_State, i);
			s << n;
		}
		break;
		
		case LUA_TBOOLEAN:
		{
			int n = lua_toboolean(m_State, i);
			s << (n ? "true" : "false");
		}
		break;
		
		case LUA_TSTRING:
		{
			char const* n = lua_tostring(m_State, i);
			s << '"';
			
			for(char const* p = n; *p; ++p)
			{
				char c = *p;
				if(c == '"')
					s << "\\\"";
				else
					s << c;
			}
			s << '"';
		}
		break;
		
		case LUA_TTABLE:
		{
			s << "{\n";
			++indent;
			size_t idx = 1;
			for(;; ++idx)
			{
				lua_rawgeti(m_State, i, idx);
				if(lua_isnil(m_State, -1))
				{
					pop(1);
					break;
				}
				
				for(int j = 0; j < indent; ++j)
					s.put('\t');
				serializeT(s, -1);
				s << ";\n";
				pop(1);
			}
			
			lua_pushnil(m_State);
			int tab = i < 0 ? i - 1 : i;
			while(lua_next(m_State, tab) != 0)
			{
				if(!lua_isnumber(m_State, -2) || lua_tointeger(m_State, -2) >= idx)
				{
					for(int j = 0; j < indent; ++j)
						s.put('\t');
					s << '[';
					serializeT(s, -2, indent);
					s << "] = ";
					serializeT(s, -1, indent);
					s << ";\n";
				}
				pop(1);
			}
			--indent;
			for(int j = 0; j < indent; ++j)
				s.put('\t');
			s << '}';
		}
		break;
		
		default: // Ignore any value we can't handle and encode a nil instead
			s << "nil";
		break;
	}
}

LuaContext::~LuaContext()
{
	if(m_State && !m_borrowed)
		lua_close(m_State);
}
