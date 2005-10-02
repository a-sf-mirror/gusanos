#ifndef LUA_TYPES_H
#define LUA_TYPES_H

struct LuaReference
{
	LuaReference()
	{
	}
	
	explicit LuaReference(int idx_)
	: idx(idx_)
	{
	}
	
	operator bool()
	{
		return idx != 0;
	}
	
	int idx;
};

#define METHOD(type_, name_, body_) \
	int l_##name_(lua_State* L_) { \
		LuaContext context(L_); \
		void* p2 = lua_touserdata (context, 1); \
		if(!p2) return 0; \
		type_* p = *static_cast<type_ **>(p2); \
		body_ }

#define LMETHOD(type_, name_, body_) \
	int l_##name_(lua_State* L_) { \
		LuaContext context(L_); \
		type_* p = static_cast<type_ *>(lua_touserdata (context, 1)); \
		if(!p) return 0; \
		body_ }
		
#define CLASS(name_, body_) { \
	lua_newtable(context); \
	lua_pushstring(context, "__index"); \
	lua_newtable(context); \
	context.tableFunctions() \
		body_ ; \
	lua_rawset(context, -3); \
	name_##MetaTable = context.createReference(); }

#define ENUM(name_, body_) { \
	lua_pushstring(context, #name_); \
	lua_newtable(context); \
	context.tableItems() \
		body_ ; \
	lua_rawset(context, LUA_GLOBALSINDEX); }
#endif //LUA_TYPES_H
